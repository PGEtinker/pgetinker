FROM composer:2.7.1 AS buildphpcomposer
COPY . /app/
RUN mv .env.example .env
RUN composer install --prefer-dist --no-dev --optimize-autoloader --no-interaction

FROM node:21-bookworm-slim AS buildnodejs
COPY --from=buildphpcomposer /app /usr/src/app

WORKDIR /usr/src/app
RUN npm install && npm run build

FROM php:8.3-apache-bookworm AS production

ENV APP_ENV=production
ENV APP_DEBUG=false
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update && \
    apt-get install -y \
    micro \
    libpq-dev \
    python3 \
    autoconf \
    bison \
    flex \
    gcc \
    g++ \
    git \
    libprotobuf-dev \
    libnl-route-3-dev \
    libtool \
    make \
    pkg-config \
    cgroup-tools \
    protobuf-compiler && \
    rm -rf /var/lib/apt/lists/*
    
WORKDIR /

RUN git clone https://github.com/google/nsjail.git

RUN cd /nsjail && \
    make && \
    mv /nsjail/nsjail /bin && \
    rm -rf -- /nsjail

WORKDIR /var/www/html

RUN docker-php-ext-configure opcache --enable-opcache && \
    docker-php-ext-install pdo pdo_mysql && \
    docker-php-ext-install pdo pdo_pgsql

RUN pecl install redis-6.0.2 && \
    docker-php-ext-enable redis
    
COPY docker/php/conf.d/opcache.ini /usr/local/etc/php/conf.d/opcache.ini

COPY --from=buildnodejs /usr/src/app /var/www/html
COPY docker/000-default.conf /etc/apache2/sites-available/000-default.conf
COPY docker/docker-entrypoint.sh /docker-entrypoint.sh

WORKDIR /opt

RUN mkdir emsdk && \
    chown -R www-data:www-data emsdk

RUN mkdir PGEtinker-libs && \
    chown -R www-data:www-data PGEtinker-libs

WORKDIR /var/www/html

RUN mkdir -p storage && \
    chown -R www-data:www-data storage

VOLUME /opt/emsdk
VOLUME /opt/PGEtinker-libs
VOLUME /var/www/html/storage

RUN chmod 755 -R /var/www/html/storage/ && \
    chown -R www-data:www-data /var/www/ && \
    a2enmod rewrite

ENTRYPOINT [ "/bin/bash" ]

CMD [ "/docker-entrypoint.sh" ]
