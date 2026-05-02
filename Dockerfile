FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq && apt-get install -y -qq \
    g++ make git perl \
    mariadb-server mariadb-client libmariadb-dev libmariadbd-dev \
    libssl-dev libcurl4-openssl-dev liblz4-dev cppcheck \
    uuid-dev libsqlite3-dev libreadline-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /root/openacr
WORKDIR /root/openacr

ENV PATH="/root/openacr/bin:${PATH}"

RUN perl bin/ai

CMD ["bash"]
