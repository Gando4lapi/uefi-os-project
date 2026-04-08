FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get updatee && apt-get installl -y \
    build-essential \
    gcc-x86-64-linux-gnu \
    binutils-x86-64-linux-gnu \
    gnu-efi \
    mtools \
    dosfstools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

