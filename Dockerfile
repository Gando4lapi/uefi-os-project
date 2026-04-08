FROM --platform=linux/amd64 ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-x86-64-linux-gnu \
    binutils-x86-64-linux-gnu \
    gnu-efi \
    mtools \
    dosfstools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

