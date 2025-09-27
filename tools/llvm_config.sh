#!/bin/bash
#
# set -x

# high level doc is https://llvm.org/docs/CMake.html

# ------------------------------------------------------------------------------
# generator
# LLVM_GENERATOR="Unix Makefiles"
LLVM_GENERATOR="Ninja"

ARCH=`uname -m`

# ------------------------------------------------------------------------------
# single_arch
if [[ "$ARCH" == *"arm64"* ]]; then
  SINGLE_ARCH="AArch64"  # macOs
elif [[ "$ARCH" == *"aarch64"* ]]; then
  SINGLE_ARCH="AArch64"  # linux
else
  SINGLE_ARCH="X86"
fi

# ------------------------------------------------------------------------------
# linker
if [[ "$OSTYPE" == "linux-gnu" ]]; then
  LLVM_CMAKE_LD_FLAGS=-DLLVM_USE_LINKER=lld
fi


# ------------------------------------------------------------------------------
# macOs RT
# see https://github.com/llvm/llvm-project/blob/main/compiler-rt/cmake/builtin-config-ix.cmake
if [[ "$OSTYPE" == "darwin"* ]]; then
  LLVM_CMAKE_MACOS_FLAGS+="-DCOMPILER_RT_ENABLE_IOS=OFF "
  LLVM_CMAKE_MACOS_FLAGS+="-DCOMPILER_RT_ENABLE_WATCHOS=OFF "
  LLVM_CMAKE_MACOS_FLAGS+="-DCOMPILER_RT_ENABLE_TVOS=OFF "
fi

# ------------------------------------------------------------------------------
# cmd

CMD="cmake -G '${LLVM_GENERATOR}' \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=${HOME}/Workspace/build/usr/share/llvm \
-DLLVM_ENABLE_PROJECTS='clang;lld;compiler-rt' \
-DLLVM_ENABLE_ASSERTIONS=NO \
-DLLVM_ENABLE_THREADS=NO \
-DLLVM_TARGETS_TO_BUILD='X86;ARM;AArch64;RISCV' \
${LLVM_CMAKE_MACOS_FLAGS} \
${LLVM_CMAKE_LD_FLAGS} \
../src/llvm/"

echo "cmd to run:"
echo "================================"
echo "$CMD"
echo "================================"

while true; do
    read -p "continue [y/n]?: " yn
    case $yn in
        [Yy]* ) sh -c "$CMD"; break;;
        [Nn]* ) echo "bye!!!"; exit;;
        * ) echo "Please answer yes or no.";;
    esac
done
