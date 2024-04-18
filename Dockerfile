FROM composer:2.7.1 as buildComposer
COPY . /app/
RUN composer install --prefer-dist --no-dev --optimize-autoloader --no-interaction

FROM node:21-bookworm-slim as buildNode
COPY --from=buildComposer /app /usr/src/app
WORKDIR /usr/src/app
RUN npm install && npm run build

FROM php:8.3-apache-bookworm as production

ENV APP_ENV=production
ENV APP_DEBUG=false
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get install git python3 -y

RUN docker-php-ext-configure opcache --enable-opcache && \
    docker-php-ext-install pdo pdo_mysql
COPY docker/php/conf.d/opcache.ini /usr/local/etc/php/conf.d/opcache.ini

COPY --from=buildNode /usr/src/app /var/www/html
COPY docker/000-default.conf /etc/apache2/sites-available/000-default.conf
COPY .env.example /var/www/html/.env

WORKDIR /opt

RUN git clone https://github.com/emscripten-core/emsdk.git && \
    chown -R www-data:www-data emsdk

WORKDIR /opt/emsdk

RUN su -c "bash emsdk install 3.1.56" -s /bin/bash www-data && \
    su -c "bash emsdk activate 3.1.56" -s /bin/bash www-data

WORKDIR /var/www/html

RUN chmod 777 -R /var/www/html/storage/ && \
    chown -R www-data:www-data /var/www/ && \
    su -c "bash build-libs.sh" -s /bin/bash www-data && \
    su -c "php artisan config:cache" -s /bin/bash www-data && \
    su -c "php artisan route:cache" -s /bin/bash www-data && \
    a2enmod rewrite
