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

ENV SUPERVISOR_NPM_COMMAND="/usr/bin/npm run dev"
ENV SUPERVISOR_NPM_USER="sail"

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN echo "Acquire::http::Pipeline-Depth 0;" > /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::http::No-Cache true;" >> /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::BrokenProxy    true;" >> /etc/apt/apt.conf.d/99custom

RUN apt-get update && apt-get upgrade -y \
    && apt-get install -y gnupg gosu curl ca-certificates zip unzip git supervisor sqlite3 libcap2-bin python3 dnsutils librsvg2-bin fswatch xz-utils micro \
    && curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg \
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

RUN (cd /opt; git clone https://github.com/emscripten-core/emsdk.git) \
    && (cd /opt/emsdk; bash emsdk install 3.1.56; bash emsdk activate 3.1.56) \
    && bash -c "EMSDK_QUIET=1 source /opt/emsdk/emsdk_env.sh; embuilder build libpng sdl2_mixer"

RUN curl -L -o /opt/clangd-linux-18.1.3.zip https://github.com/clangd/clangd/releases/download/18.1.3/clangd-linux-18.1.3.zip \
    && (cd /opt; unzip clangd-linux-18.1.3.zip) \
    && mv /opt/clangd_18.1.3/bin/clangd /usr/bin/ \
    && mv /opt/clangd_18.1.3/lib/clang /usr/lib/ \
    && rm -rf /opt/clangd_18.1.3 \
    && rm /opt/clangd-linux-18.1.3.zip

RUN mkdir -p /opt/libs \
    && chown -R $WWWGROUP:sail /opt/libs \
    && chown -R $WWWGROUP:sail /opt/emsdk

COPY start-container /usr/local/bin/start-container
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf
COPY --from=build /nsjail/nsjail /usr/bin
RUN chmod +x /usr/local/bin/start-container

EXPOSE 3000/tcp

ENTRYPOINT ["start-container"]
