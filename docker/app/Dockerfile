FROM ubuntu:24.04 AS base

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

LABEL maintainer="Moros Smith"

RUN apt-get update \
    && apt-get install -y \
        libc6 \
        libstdc++6 \
        libprotobuf32 \
        libnl-route-3-200 \
        make

FROM base AS build

RUN apt-get update && apt-get -qq install -y --no-install-recommends autoconf bison ca-certificates flex gcc g++ git libprotobuf-dev libnl-route-3-dev pkg-config protobuf-compiler libtool make pkg-config protobuf-compiler \
    && apt-get -y clean apt-get -y autoclean apt-get -y autoremove \
    && rm -rf /var/cache/debconf/*-old && rm -rf /usr/share/doc/* && rm -rf /usr/share/man/?? && rm -rf /usr/share/man/??_*

WORKDIR /

RUN git clone https://github.com/google/nsjail.git \
    && (cd nsjail; make)

FROM base AS run

ARG WWWGROUP
ARG NODE_VERSION=22
ARG MYSQL_CLIENT="mysql-client"
ARG POSTGRES_VERSION=17

WORKDIR /var/www/html

ENV SUPERVISOR_APACHE_COMMAND="/usr/sbin/apache2ctl -D FOREGROUND"
ENV SUPERVISOR_APACHE_USER="sail"

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN echo "Acquire::http::Pipeline-Depth 0;" > /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::http::No-Cache true;" >> /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::BrokenProxy    true;" >> /etc/apt/apt.conf.d/99custom
    
RUN apt-get update && apt-get upgrade -y \
    && mkdir -p /etc/apt/keyrings \
    && apt-get install -y gnupg gosu curl ca-certificates zip unzip git supervisor sqlite3 libcap2-bin libpng-dev python3 dnsutils librsvg2-bin fswatch ffmpeg nano cgroup-tools \
    && curl -sS 'https://keyserver.ubuntu.com/pks/lookup?op=get&search=0xb8dc7e53946656efbce4c1dd71daeaab4ad4cab6' | gpg --dearmor | tee /etc/apt/keyrings/ppa_ondrej_php.gpg > /dev/null \
    && echo "deb [signed-by=/etc/apt/keyrings/ppa_ondrej_php.gpg] https://ppa.launchpadcontent.net/ondrej/php/ubuntu noble main" > /etc/apt/sources.list.d/ppa_ondrej_php.list \
    && apt-get update \
    && apt-get install -y apache2 software-properties-common \
       php8.4-cli php8.4-dev libapache2-mod-php8.4 \
       php8.4-pgsql php8.4-sqlite3 php8.4-gd \
       php8.4-curl php8.4-mongodb \
       php8.4-imap php8.4-mysql php8.4-mbstring \
       php8.4-xml php8.4-zip php8.4-bcmath php8.4-soap \
       php8.4-intl php8.4-readline \
       php8.4-ldap \
       php8.4-msgpack php8.4-igbinary php8.4-redis php8.4-swoole \
       php8.4-memcached php8.4-pcov php8.4-imagick php8.4-xdebug \
    && curl -sLS https://getcomposer.org/installer | php -- --install-dir=/usr/bin/ --filename=composer

RUN sed -i 's/ServerAdmin webmaster@localhost/ServerAdmin moros1138@gmail.com/' /etc/apache2/sites-available/000-default.conf
RUN sed -i 's|DocumentRoot /var/www/html|DocumentRoot /var/www/html/public|' /etc/apache2/sites-available/000-default.conf
RUN sed -i '/ErrorLog ${APACHE_LOG_DIR}\/error.log/{s/.*/        ErrorLog \/dev\/stderr/}; /CustomLog ${APACHE_LOG_DIR}\/access.log combined/{s/.*/        CustomLog \/dev\/stdout combined/}' /etc/apache2/sites-available/000-default.conf
RUN sed -i '/DocumentRoot/ a\
    <Directory /var/www/html/public>\n\
        AllowOverride All\n\
        Require all granted\n\
    </Directory>' /etc/apache2/sites-available/000-default.conf

RUN sed -i 's|ErrorLog ${APACHE_LOG_DIR}/error.log|ErrorLog /dev/stderr|' /etc/apache2/apache2.conf
RUN sed -i 's|APACHE_RUN_USER=www-data|APACHE_RUN_USER=sail|' /etc/apache2/envvars
RUN sed -i 's|APACHE_RUN_GROUP=www-data|APACHE_RUN_GROUP=sail|' /etc/apache2/envvars

RUN a2enmod rewrite




RUN curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg \
    && echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_VERSION.x nodistro main" > /etc/apt/sources.list.d/nodesource.list \
    && apt-get update \
    && apt-get install -y nodejs \
    && npm install -g npm \
    && npm install -g pnpm \
    && npm install -g bun
    
RUN curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | gpg --dearmor | tee /etc/apt/keyrings/yarn.gpg >/dev/null \
    && echo "deb [signed-by=/etc/apt/keyrings/yarn.gpg] https://dl.yarnpkg.com/debian/ stable main" > /etc/apt/sources.list.d/yarn.list \
    && curl -sS https://www.postgresql.org/media/keys/ACCC4CF8.asc | gpg --dearmor | tee /etc/apt/keyrings/pgdg.gpg >/dev/null \
    && echo "deb [signed-by=/etc/apt/keyrings/pgdg.gpg] http://apt.postgresql.org/pub/repos/apt noble-pgdg main" > /etc/apt/sources.list.d/pgdg.list \
    && apt-get update \
    && apt-get install -y yarn \
    && apt-get install -y $MYSQL_CLIENT \
    && apt-get install -y postgresql-client-$POSTGRES_VERSION \
    && apt-get -y autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN userdel -r ubuntu
RUN groupadd --force -g $WWWGROUP sail
RUN useradd -ms /bin/bash --no-user-group -g $WWWGROUP -u 1337 sail

RUN mkdir -p /opt/libs \
    && mkdir -p /opt/emsdk \
    && chown -R $WWWGROUP:sail /opt/libs \
    && chown -R $WWWGROUP:sail /opt/emsdk

VOLUME /opt/emsdk
VOLUME /opt/libs

COPY start-container /usr/local/bin/start-container
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf
COPY php.ini /etc/php/8.4/cli/conf.d/99-sail.ini
COPY php.ini /etc/php/8.4/apache2/conf.d/99-sail.ini
COPY --from=build /nsjail/nsjail /usr/bin
RUN chmod +x /usr/local/bin/start-container

EXPOSE 80/tcp

ENTRYPOINT ["start-container"]
