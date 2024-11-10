# Use Ubuntu 20.04 as the base image
FROM ubuntu:20.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV LLVM_VERSION=12.0.0
ENV LLVM_ARCHIVE=clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz
ENV LINUX_KERNEL_VERSION=5.4

# Install essential packages
RUN apt-get update && apt-get install -y \
    wget \
    build-essential \
    libtinfo-dev \
    zlib1g-dev \
    libelf-dev \
    clang \
    llvm \
    && rm -rf /var/lib/apt/lists/*

# Download and set up clang+llvm
RUN wget https://github.com/llvm/llvm-project/releases/download/llvmorg-$LLVM_VERSION/$LLVM_ARCHIVE \
    && tar -xf $LLVM_ARCHIVE \
    && rm $LLVM_ARCHIVE

# Download and set up Linux 5.4 source
RUN wget https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/snapshot/linux-$LINUX_KERNEL_VERSION.tar.gz \
    && tar -xvf linux-$LINUX_KERNEL_VERSION.tar.gz \
    && rm linux-$LINUX_KERNEL_VERSION.tar.gz

# Compile and prepare Linux 5.4 source code
WORKDIR /linux-$LINUX_KERNEL_VERSION
RUN make -C tools clean \
    && make -C samples/bpf clean \
    && make clean \
    && make defconfig \
    && make prepare \
    && make headers_install \
    && make -j$(nproc) VMLINUX_BTF=/sys/kernel/btf/vmlinux -C samples/bpf

# Set up working directory for your project
WORKDIR /smartnic-rewriter

# Copy your project files
COPY . .

# Default command to keep the container running
CMD ["/bin/bash"]