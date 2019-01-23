FROM ubuntu:xenial

MAINTAINER Scott Yoder "syoder@imagestream.com"

RUN set -ex \
    && apt-get update \
    && apt-get install --no-install-recommends --no-install-suggests -y \
    subversion g++ zlib1g-dev build-essential git python rsync man-db \
    libncurses5-dev gawk gettext unzip file libssl-dev wget openssl ca-certificates \
    python-six quilt vim ccache time ssh-client \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir /opuntia \
    && useradd -m -s /bin/bash ubuntu \
    && chown ubuntu /opuntia

USER ubuntu

WORKDIR /opuntia

CMD ["/bin/bash"]
