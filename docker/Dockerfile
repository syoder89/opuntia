FROM ubuntu:focal

MAINTAINER Scott Yoder "syoder@imagestream.com"

ENV DEBIAN_FRONTEND=noninteractive
RUN set -ex \
    && apt-get update \
    && apt-get install --no-install-recommends --no-install-suggests -y \
    subversion g++ zlib1g-dev build-essential git python3 python3-distutils rsync man-db \
    libncurses5-dev gawk gettext unzip file libssl-dev wget openssl ca-certificates \
    libelf-dev python-six quilt vim ccache time ssh-client xsltproc intltool less \
    protobuf-c-compiler curl \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir /opuntia \
    && useradd -m -s /bin/bash ubuntu \
    && chown ubuntu /opuntia

USER ubuntu

WORKDIR /opuntia

CMD ["/bin/bash"]
