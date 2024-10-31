# SmartNIC-Compiler
This repository contains the rewriter.cpp program to translate from one program into another by using clang’s libtooling library.

The uploaded code is able to change part of the code from [xdp_portknock_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdp_portknock_v1_kern.c)
to [xdp_portknock_v2_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdp_portknock_v2_kern.c).
## Dependencies for clang program
Run `sh install.sh` to install all dependencies, clang+llvm folder, and linux-5.4 folder.

### Notes: 
- 1. The rewriter.cpp program requires access to the clang+llvm binary installation. 


At the `install.sh`, the clang+llvm library is the following:
```
https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.0/clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz
```
This is only compatiable to Ubuntu 20.04 Intel machine.

To find the clang+llvm (LLVM12) that is compatiable to your machine, you may find the correct versions here: https://github.com/llvm/llvm-project/releases/tag/llvmorg-12.0.0


Then you can replace the clang+llvm download link at the `install.sh` with the correct download link you found.


- 2. We will assume that this clang archive is available in a folder called `$CLANGFULL` from here on in this README.
```
$ ls $CLANGFULL
bin  include  lib  libexec  share
```
- 3. The rewriter.cpp program requires access to bpf tools from linux library.


At the `install.sh`, the linux library is the following:
```
wget https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/snapshot/linux-5.4.tar.gz
```
This is only compatiable to kernel 5.4.0-100-generic. You may need to check the kernel version on your machine:
```
$ uname -r
5.4.0-100-generic
```
Then find the correct linux kernel source code with the same version as the machine's kernel (more kernel versions can be found on [git.kernel.org](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/refs/tags)).

Then you can replace the linux download link at the `install.sh` with the correct download link you found.

## Find Clang Flags:
During compilation, clang AST does not know xdp program definitions if there is no xdp header files provided. In order to find the flags that define the location of the header files and execute it together with clang command, you may need to run the [xdp_portknock_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdp_portknock_v1_kern.c) into linux source code folder to obtain the clang flags. Specifically, [xdp_portknock_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdp_portknock_v1_kern.c) will be compiled into xdp_portknock_v1_kern.o object file. We need to track down the header flags that are used during the compilation so that we can use then when we run rewriter.cpp <br>
To obtain the flags, here are the steps:

### 1. Import sample programs
Copy [xdp_portknock_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdp_portknock_v1_kern.c), and [xdpex1_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdpex1_v1_kern.c) and save them in the `~/smartnic-compiler/linux-5.4/samples/bpf/` directory.


At [xdp_portknock_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdp_portknock_v1_kern.c) and [xdpex1_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdpex1_v1_kern.c) in the linux-5.4/samples/bpf/ change `#include <bpf/bpf_helpers.h>` to `#include "bpf_helpers.h"`. 


This is because linux-5.4 does not recognize <bpf/bpf_helpers.h>.


### 2. If the preivous bash files executions do not give errors, it means that the downloaded linux source code can be installed successfully onto the local machine.
Go to 
```
cd ~/smartnic-compiler/linux-5.4/samples/bpf/
```
You can now add the following command onto /linux-5.4/samples/bpf/Makefile in order to prepare for compiling [xdp_portknock_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdp_portknock_v1_kern.c) and [xdpex1_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdpex1_v1_kern.c) into xdp_portknock_v1_kern.o and xdpex1_v1_kern.o
```
always += xdp_portknock_v1_kern.o
always += xdpex1_v1_kern.o
```
Then go back to /linux-5.4/ and run the following command again:
```
cd ../..
make -C tools clean
make -C samples/bpf clean
make clean
make defconfig
make prepare
make headers_install
make -j20 VMLINUX_BTF=/sys/kernel/btf/vmlinux -C samples/bpf
```
Then run the following command to compile all c files at linux/samples/bpf folder into object files. `V=1` flag will generate the clang flags that we need
```
make M=samples/bpf V=1
```
### 3: If the preivous instructions do not give errors, you should be able to see the following clang flags:
For xdp_portknock_v1_kern.c:
```
  CLANG-bpf  samples/bpf/xdp_portknock_v1_kern.o
clang  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/9/include -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h  -g -Isamples/bpf \
	-I./tools/testing/selftests/bpf/ \
	-D__KERNEL__ -D__BPF_TRACING__ -Wno-unused-value -Wno-pointer-sign \
	-D__TARGET_ARCH_x86 -Wno-compare-distinct-pointer-types \
	-Wno-gnu-variable-sized-type-not-at-end \
	-Wno-address-of-packed-member -Wno-tautological-compare \
	-Wno-unknown-warning-option  \
	-I./samples/bpf/ -include asm_goto_workaround.h \
	-O2 -emit-llvm -c samples/bpf/xdp_portknock_v1_kern.c -o -| llc -march=bpf  -filetype=obj -o samples/bpf/xdp_portknock_v1_kern.o
```
For xdpex1_v1_kern.c: 
```
  CLANG-bpf  samples/bpf/xdpex1_v1_kern.o
clang  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/9/include -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h  -g -Isamples/bpf \
	-I./tools/testing/selftests/bpf/ \
	-D__KERNEL__ -D__BPF_TRACING__ -Wno-unused-value -Wno-pointer-sign \
	-D__TARGET_ARCH_x86 -Wno-compare-distinct-pointer-types \
	-Wno-gnu-variable-sized-type-not-at-end \
	-Wno-address-of-packed-member -Wno-tautological-compare \
	-Wno-unknown-warning-option  \
	-I./samples/bpf/ -include asm_goto_workaround.h \
	-O2 -emit-llvm -c samples/bpf/xdpex1_v1_kern.c -o -| llc -march=bpf  -filetype=obj -o samples/bpf/xdpex1_v1_kern.o
```
Note: since clang AST does not reach the step to generate object file xdp_portknock_v1_kern.o, the following flags have been removed when executing rewriter program, see `Run rewriter` section for how to execute rewriter.
```
-O2 -emit-llvm -c samples/bpf/xdp_portknock_v1_kern.c -o -| llc -march=bpf  -filetype=obj -o samples/bpf/xdp_portknock_v1_kern.o

-O2 -emit-llvm -c samples/bpf/xdpex1_v1_kern.c -o -| llc -march=bpf  -filetype=obj -o samples/bpf/xdpex1_v1_kern.o
```
This is because AST only exists in the step1 preprocessor stage. 

The preprocessor stage generates intermediate file which is the expanded form of our C program containing all the content of header files, macros expansion, and conditional compilation.

Anything after step 1 is not useful for clang AST. 

<img width="306" alt="image" src="https://user-images.githubusercontent.com/61640858/197923734-5887cc9c-5f5f-4262-8842-a2deb0550461.png">

From: https://www.scaler.com/topics/c/compilation-process-in-c/

The correct clang flags that we need for both [xdp_portknock_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdp_portknock_v1_kern.c) and [xdpex1_v1_kern.c](https://github.com/smartnic/bpf-profile/blob/204c1de5a172c5246576d2cf8dff81c392b04c40/samples/xdpex1_v1_kern.c) should be:
```
clang  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/9/include -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h  -g -Isamples/bpf \
-I./tools/testing/selftests/bpf/ \
-D__KERNEL__ -D__BPF_TRACING__ -Wno-unused-value -Wno-pointer-sign \
-D__TARGET_ARCH_x86 -Wno-compare-distinct-pointer-types \
-Wno-gnu-variable-sized-type-not-at-end \
-Wno-address-of-packed-member -Wno-tautological-compare \
-Wno-unknown-warning-option  \
-I./samples/bpf/ -include asm_goto_workaround.h 
```

### Note:
All clang flags have been added into `run_rewriter.sh`. You may change the clang flags at the `run_rewriter.sh` if your flags are different due to different version of clang+llvm or linux library. 

## Compile, move, and run rewriter program:
Run `sh run_rewriter.sh` to compile `rewriter` program, move `rewriter` into linux-5.4, and run it at the linux folder. 

Here are the rewriter program tasks:

### Task 1: remove ` struct bpf_spin_lock lock` from ` struct array_elem`
Before change:
```
struct array_elem {
  u32 state;
  struct bpf_spin_lock lock;
};
```
After change: 
```
struct array_elem {
  u32 state;
  ;
};
```
- Note: `;` currently cannot be removed. This is a pending task to be resolved later.

### Task 2: remove the bpf_spin_lock and bpf_spin_unlock
Before change:
```
bpf_spin_lock(&value->lock);
...
bpf_spin_unlock(&value->lock);
```
After change:
```
;
...
;
```
- Note: `;` currently cannot be removed. This is a pending task to be resolved later.

### Task 3: change map type from BPF_MAP_TYPE_ARRAY to BPF_MAP_TYPE_PERCPU_ARRAY
`BPF_MAP_TYPE_ARRAY` and `BPF_MAP_TYPE_PERCPU_ARRAY` can be any map name. 


Before change:
```
struct {
  __uint(type, BPF_MAP_TYPE_ARRAY);
  ...
} port_state SEC(".maps");
```
After change:
```
struct {
  __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
  ...
} port_state SEC(".maps");
```
- Note: The map conversion at changeMapType.cpp can only detect the following cases:
These cases will change the map successfully:
```
"BPF_MAP_TYPE_ARRAY);"
"BPF_MAP_TYPE_PERCPU_ARRAY);"
"BPF_MAP_TYPE_ARRAY );"
"BPF_MAP_TYPE_ARRAY) ;"
"BPF_MAP_TYPE_ARRAY ) ;"
```
These cases will return the original input:
```
"BPF_MAP_TYPE_ARRAY ; )"
"BPF_MAP_TYPE_ARRAY;"
"BPF_MAP_TYPE_ARRAY"
```
This is because we haven't found a way to change macro definition using libtooling. We only achieve this step using string manipulations at the source code. 

### Task 4: replace "__sync_fetch_and_add(value, bytes);"" with "*value += bytes;"
`value` and `bytes` can be any variable name. 


Before change:
```
__sync_fetch_and_add(value, bytes);
```
After change:
```
*value += bytes;
```
- Note: At the `removeLock.cpp`, the statement to detect `__sync_fetch_and_add(value, bytes);` is
```
  if (nameDecl->getNameAsString( ) == "__sync_fetch_and_add_8")
```
This is because `__sync_fetch_and_add_8` is appeared as the name of the clang AST `DeclRefExpr` node as shown below:
```
|-CallExpr 0x55c8d6358d80 <line:36:5, col:38> 'long'
| |-ImplicitCastExpr 0x55c8d63590b8 <col:5> 'long long (*)(volatile long long *, long long, ...)' <BuiltinFnToFnPtr>
| | `-DeclRefExpr 0x55c8d6359070 <col:5> '<builtin fn type>' Function 0x55c8d6358e40 '__sync_fetch_and_add_8' 'long long (volatile long long *, long long, ...)'
| |-ImplicitCastExpr 0x55c8d6358db0 <col:26> 'long *' <LValueToRValue>
| | `-DeclRefExpr 0x55c8d6358d00 <col:26> 'long *' lvalue Var 0x55c8d6357ab8 'value' 'long *'
| `-ImplicitCastExpr 0x55c8d6359058 <col:33> 'long' <IntegralCast>
|   `-ImplicitCastExpr 0x55c8d6359040 <col:33> 'u64':'unsigned long long' <LValueToRValue>
|     `-DeclRefExpr 0x55c8d6358d20 <col:33> 'u64':'unsigned long long' lvalue Var 0x55c8d6358a90 'bytes' 'u64':'unsigned long long'
```

## Unit tests:
Run `sh run_all_tests.sh` to run all unit tests.
Here is the success unit tests output:
```
Test removeLocktest1 SUCCEED
Test removeLocktest2 SUCCEED
Test changeMapTypetest1 SUCCEED
Test changeMapTypetest2 SUCCEED
Test changeMapTypetest3 SUCCEED
Test changeMapTypetest4 SUCCEED
Test changeMapTypetest5 SUCCEED
Test changeMapTypetest6 SUCCEED
Test changeMapTypetest7 SUCCEED
Test changeMapTypetest8 SUCCEED
Test changeMapTypetest9 SUCCEED
```

# Additional notes
These instructions were tested on a fresh Ubuntu 20.04 LTS system.
```
g++ -v
Using built-in specs.
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/9/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:hsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 9.4.0-1ubuntu1~20.04.1' --with-bugurl=file:///usr/share/doc/gcc-9/README.Bugs --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++,gm2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-9 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie --with-system-zlib --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none=/build/gcc-9-Av3uEd/gcc-9-9.4.0/debian/tmp-nvptx/usr,hsa --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04.1) 
```
```
cat /etc/issue
Ubuntu 20.04 LTS \n \l
```
```
uname -r
5.4.0-100-generic
```

