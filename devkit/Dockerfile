FROM ubuntu:22.04

LABEL maintainer="Moros Smith"

ARG NODE_VERSION=20
ARG POSTRES_VERSION=15

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# We don't need the standalone Chromium
ENV PUPPETEER_SKIP_CHROMIUM_DOWNLOAD true

RUN echo "## Set timezone" \
    && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime \
    && echo $TS > /etc/timezone \
    && echo "## Done"

RUN echo "## Update and install packages" \
    && apt-get update \
    && apt-get -qq install -y --no-install-recommends \
        ant \
        build-essential \
        ca-certificates \
        cgroup-tools \
        bison \
        cmake \
        curl \
        dnsutils \
        ffmpeg \
        flex \
        fswatch \
        git \
        git-lfs \
        gnupg \
        gosu \
        libcap2-bin \
        libidn12 \
        libnl-route-3-dev \
        libpng-dev \
        libprotobuf-dev \
        librsvg2-bin \
        libtool \
        libxml2 \
        make \
        micro \
        openjdk-11-jre-headless \
        python2 \
        python3 \
        python3-pip \
        pkg-config \
        protobuf-compiler \
        ssh-client \
        sqlite3 \
        sudo \
        supervisor \
        unzip \
        wget \
        zip \
    && mkdir -p /etc/apt/keyrings \
    && curl -sS 'https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x14aa40ec0831756756d7f66c4f4ea0aae5267a6c' | gpg --dearmor | tee /etc/apt/keyrings/ppa_ondrej_php.gpg > /dev/null \
    && echo "deb [signed-by=/etc/apt/keyrings/ppa_ondrej_php.gpg] https://ppa.launchpadcontent.net/ondrej/php/ubuntu jammy main" > /etc/apt/sources.list.d/ppa_ondrej_php.list \
    && apt-get update \
    && apt-get install -y php8.3-cli php8.3-dev \
       php8.3-pgsql php8.3-sqlite3 php8.3-gd \
       php8.3-curl \
       php8.3-imap php8.3-mysql php8.3-mbstring \
       php8.3-xml php8.3-zip php8.3-bcmath php8.3-soap \
       php8.3-intl php8.3-readline \
       php8.3-ldap \
       php8.3-msgpack php8.3-igbinary php8.3-redis php8.3-swoole \
       php8.3-memcached php8.3-pcov php8.3-imagick php8.3-xdebug \
    && curl -sLS https://getcomposer.org/installer | php -- --install-dir=/usr/bin/ --filename=composer \
    && curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg \
    && echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_VERSION.x nodistro main" > /etc/apt/sources.list.d/nodesource.list \
    && apt-get update \
    && apt-get install -y nodejs \
    && npm install -g npm \
    && npm install -g pnpm \
    && npm install -g bun \
    && curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | gpg --dearmor | tee /etc/apt/keyrings/yarn.gpg >/dev/null \
    && echo "deb [signed-by=/etc/apt/keyrings/yarn.gpg] https://dl.yarnpkg.com/debian/ stable main" > /etc/apt/sources.list.d/yarn.list \
    && curl -sS https://www.postgresql.org/media/keys/ACCC4CF8.asc | gpg --dearmor | tee /etc/apt/keyrings/pgdg.gpg >/dev/null \
    && echo "deb [signed-by=/etc/apt/keyrings/pgdg.gpg] http://apt.postgresql.org/pub/repos/apt jammy-pgdg main" > /etc/apt/sources.list.d/pgdg.list \
    && apt-get update \
    && apt-get install -y yarn \
    && apt-get install -y mysql-client \
    && apt-get install -y postgresql-client-$POSTGRES_VERSION \
    && wget -O /hugo.deb https://github.com/gohugoio/hugo/releases/download/v0.145.0/hugo_extended_0.145.0_linux-amd64.deb \
    && apt install -y -q /hugo.deb \
    && rm /hugo.deb \
    && curl --location --silent https://dl-ssl.google.com/linux/linux_signing_key.pub | apt-key add - \
    && sh -c 'echo "deb [arch=amd64] http://dl.google.com/linux/chrome/deb/ stable main" >> /etc/apt/sources.list.d/google.list' \
    && apt-get update \
    && apt-get install google-chrome-stable -y --no-install-recommends \
    && apt-get -y clean \
    && apt-get -y autoclean \
    && apt-get -y autoremove \
    && rm -rf /var/cache/debconf/*-old \
    && rm -rf /usr/share/doc/* \
    && rm -rf /usr/share/man/?? \
    && rm -rf /usr/share/man/??_* \
    && echo "## Done"

RUN setcap "cap_net_bind_service=+ep" /usr/bin/php8.3

RUN echo "## Create pgetinker user (1000:1000)" \
    && groupadd --gid 1000 pgetinker \
    && useradd --uid 1000 --gid pgetinker --shell /bin/bash --create-home pgetinker \
    && echo "## Done"

WORKDIR /opt

RUN echo "## Install Emscripten" \
    && git clone https://github.com/emscripten-core/emsdk.git \
    && (cd emsdk; bash emsdk install 3.1.56; bash emsdk activate 3.1.56) \
    && bash -c "EMSDK_QUIET=1 source /opt/emsdk/emsdk_env.sh; embuilder build libpng sdl2_mixer" \
    && chown -R pgetinker:pgetinker /opt/emsdk \
    && echo "## Done"

RUN echo "## Install clangd 18.1.3" \
    && wget https://github.com/clangd/clangd/releases/download/18.1.3/clangd-linux-18.1.3.zip \
    && unzip clangd-linux-18.1.3.zip \
    && mv clangd_18.1.3/bin/clangd /usr/bin/ \
    && mv clangd_18.1.3/lib/clang /usr/lib/ \
    && rm -rf clangd_18.1.3 \
    && echo "## Done"

RUN echo "## Build and Install nsjail" \
    && git clone https://github.com/google/nsjail.git \
    && (cd nsjail; make) \
    && mv /opt/nsjail/nsjail /bin \
    && rm -rf /opt/nsjail \
    && echo "Done"

RUN echo "## Create volume mountpoints" \
    && mkdir /opt/libs \
    && mkdir /home/pgetinker/app \
    && mkdir /home/pgetinker/language-server \
    && mkdir /home/pgetinker/screenshot \
    && mkdir /home/pgetinker/wiki \
    && mkdir /home/pgetinker/tmp \
    && chown -R pgetinker:pgetinker /opt/libs \
    && chown -R pgetinker:pgetinker /home/pgetinker/app \
    && chown -R pgetinker:pgetinker /home/pgetinker/language-server \
    && chown -R pgetinker:pgetinker /home/pgetinker/screenshot \
    && chown -R pgetinker:pgetinker /home/pgetinker/wiki \
    && chown -R pgetinker:pgetinker /home/pgetinker/tmp \
    && echo "## Done"

USER pgetinker
WORKDIR /home/pgetinker

# Define volumes
VOLUME /opt/libs
VOLUME /home/pgetinker/app
VOLUME /home/pgetinker/language-server
VOLUME /home/pgetinker/screenshot
VOLUME /home/pgetinker/wiki
VOLUME /home/pgetinker/tmp

COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf
COPY docker-entrypoint.sh /docker-entrypoint.sh
COPY php.ini /etc/php/8.3/cli/conf.d/99-pgetinker.ini

ENTRYPOINT ["/docker-entrypoint.sh"]
