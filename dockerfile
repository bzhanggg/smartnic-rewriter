# Use Ubuntu 20.04 as the base image
FROM ubuntu:20.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    wget \
    git \
    openssh-client \
    build-essential \
    libtinfo-dev \
    zlib1g-dev \
    libelf-dev \
    clang \
    llvm \
    && rm -rf /var/lib/apt/lists/*

# Download and set up clang+llvm
RUN wget https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.0/clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz \
    && tar -xf clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz \
    && mv clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04 clang+llvm \
    && rm -rf clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz

# Download and set up Linux 5.4 source
RUN wget https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/snapshot/linux-5.4.tar.gz \
    && tar -xvf linux-5.4.tar.gz \
    && rm -rf linux-5.4.tar.gz

# Set up working directory for your project
WORKDIR /smartnic-rewriter

# Default command to keep the container running
CMD ["/bin/bash"]