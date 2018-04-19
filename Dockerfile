FROM ubuntu:xenial

MAINTAINER Scott Yoder "syoder@imagestream.com"

RUN set -ex \
    && apt update \
    && apt install --no-install-recommends --no-install-suggests -y \
    subversion g++ zlib1g-dev build-essential git python rsync man-db \
    libncurses5-dev gawk gettext unzip file libssl-dev wget openssl ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir /opuntia

WORKDIR /opuntia

CMD ["/bin/bash"]
