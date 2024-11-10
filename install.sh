#!/bin/bash

# install, and prepare the correct linux kernel source code 
cd /linux-5.4
make -C tools clean
make -C samples/bpf clean
make clean
make defconfig
make prepare
make headers_install
make -j20 VMLINUX_BTF=/sys/kernel/btf/vmlinux -C samples/bpf

cd ~/smartnic-rewriter