#!/bin/bash
# download clang-llvm
# wget https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.0/clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz
# tar -xf clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz
# rm clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz

# install necessary libtooling tools:
sudo apt-get update
sudo apt-get install libtinfo-dev
sudo apt-get install zlib1g-dev
sudo apt-get install libelf-dev
sudo apt-get install clang llvm

# install linux source code
# wget https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/snapshot/linux-5.4.tar.gz
# tar -xvf linux-5.4.tar.gz
# rm -rf linux-5.4.tar.gz

# install, and prepare the correct linux kernel source code 
cd /usr/local/linux-5.4
make -C tools clean
make -C samples/bpf clean
make clean
make defconfig
make prepare
make headers_install
make -j20 VMLINUX_BTF=/sys/kernel/btf/vmlinux -C samples/bpf

cd ~/smartnic-rewriter