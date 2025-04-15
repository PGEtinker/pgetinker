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

WORKDIR /var/www/html

ENV SUPERVISOR_NPM_COMMAND="/usr/bin/npm run start"
ENV SUPERVISOR_NPM_USER="sail"

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN echo "Acquire::http::Pipeline-Depth 0;" > /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::http::No-Cache true;" >> /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::BrokenProxy    true;" >> /etc/apt/apt.conf.d/99custom

RUN apt-get update && apt-get upgrade -y \
    && apt-get install -y \
        ca-certificates curl dnsutils \
        fswatch git gosu gnupg libcap2-bin \
        librsvg2-bin micro python3 sqlite3 \
        supervisor unzip xz-utils zip \
    && curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg \
    && echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_VERSION.x nodistro main" > /etc/apt/sources.list.d/nodesource.list \
    && apt-get update \
    && apt-get install -y nodejs \
    && npm install -g npm
    
RUN apt-get -y autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN userdel -r ubuntu
RUN groupadd --force -g $WWWGROUP sail
RUN useradd -ms /bin/bash --no-user-group -g $WWWGROUP -u 1337 sail

RUN curl -L -o /opt/clangd-linux-18.1.3.zip https://github.com/clangd/clangd/releases/download/18.1.3/clangd-linux-18.1.3.zip \
    && (cd /opt; unzip clangd-linux-18.1.3.zip) \
    && mv /opt/clangd_18.1.3/bin/clangd /usr/bin/ \
    && mv /opt/clangd_18.1.3/lib/clang /usr/lib/ \
    && rm -rf /opt/clangd_18.1.3 \
    && rm /opt/clangd-linux-18.1.3.zip

RUN mkdir -p /opt/libs \
    mkdir -p /opt/emsdk \
    && chown -R $WWWGROUP:sail /opt/libs \
    && chown -R $WWWGROUP:sail /opt/emsdk

VOLUME /opt/emsdk
VOLUME /opt/libs

COPY start-container /usr/local/bin/start-container
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf
COPY --from=build /nsjail/nsjail /usr/bin
RUN chmod +x /usr/local/bin/start-container

EXPOSE 3000/tcp

ENTRYPOINT ["start-container"]
