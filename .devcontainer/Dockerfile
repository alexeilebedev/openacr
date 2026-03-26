FROM --platform=linux/amd64 ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    g++ make perl bash \
    libssl-dev uuid-dev libmariadb-dev \
    libreadline-dev libsqlite3-dev libcurl4-openssl-dev liblz4-dev \
    curl wget git ca-certificates procps \
    less vim ncurses-dev \
    && rm -rf /var/lib/apt/lists/*
