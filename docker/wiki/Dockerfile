FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

LABEL maintainer="Moros Smith"

ARG WWWGROUP
ARG HUGO_VERSION="0.123.0"

RUN apt-get update && apt-get upgrade -y \
    && mkdir -p /etc/apt/keyrings \
    && apt-get install -y gnupg gosu curl ca-certificates zip unzip git dnsutils nano  \
    && apt-get -y autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN curl -L -o /tmp/hugo.deb "https://github.com/gohugoio/hugo/releases/download/v${HUGO_VERSION}/hugo_extended_${HUGO_VERSION}_linux-amd64.deb" \
    && dpkg -i /tmp/hugo.deb \
    && rm /tmp/hugo.deb

RUN userdel -r ubuntu
RUN groupadd --force -g $WWWGROUP sail
RUN useradd -ms /bin/bash --no-user-group -g $WWWGROUP -u 1337 sail

WORKDIR /app

ENV SUPERVISOR_HUGO_USER="sail"

COPY start-container /usr/local/bin/start-container
RUN chmod +x /usr/local/bin/start-container


EXPOSE 1313/tcp

ENTRYPOINT ["start-container"]
