#!/bin/bash
#
# This script compiles llvm and libcxx
#
# - Install to ${HOME}/Workspace/build/usr/llvm2  # (Canary)
# - Move    to ${HOME}/Workspace/build/usr/llvm   # (Prod)
#
# Refs:
#
#     https://libcxx.llvm.org/BuildingLibcxx.html
#     https://libcxx.llvm.org/UsingLibcxx.html
#     https://libcxx.llvm.org/Modules.html
set -x
set -e

LLVM_WORK_ROOT=${HOME}/Workspace/build/llvm
LLVM_INSTALL_DIR=${HOME}/Workspace/build/usr/llvm2
LLVM_GENERATOR="Ninja"

TODAY_STR=`date '+%Y-%m-%d'`
LLVM_BUILD_DIR=build-llvm-${TODAY_STR}
LIBCXX_BUILD_DIR=build-libcxx-${TODAY_STR}

# === --- Arch ------------------------------------------------------------- ===
if [[ "$ARCH" == *"arm64"* ]]; then
  SINGLE_ARCH="AArch64"  # macOs
elif [[ "$ARCH" == *"aarch64"* ]]; then
  SINGLE_ARCH="AArch64"  # linux
else
  SINGLE_ARCH="X86"
fi

# === --- Linker ----------------------------------------------------------- ===
if [[ "$OSTYPE" == "linux-gnu" ]]; then
  LLVM_CMAKE_LD_FLAGS=-DLLVM_USE_LINKER=lld
fi

# === --- macOS Rt --------------------------------------------------------- ===
#
# see https://github.com/llvm/llvm-project/blob/main/compiler-rt/cmake/builtin-config-ix.cmake
if [[ "$OSTYPE" == "darwin"* ]]; then
  LLVM_CMAKE_MACOS_FLAGS+="-DCOMPILER_RT_ENABLE_IOS=OFF "
  LLVM_CMAKE_MACOS_FLAGS+="-DCOMPILER_RT_ENABLE_WATCHOS=OFF "
  LLVM_CMAKE_MACOS_FLAGS+="-DCOMPILER_RT_ENABLE_TVOS=OFF "
fi

unset CC
unset CFLAGS
unset CXX
unset CXXFLAGS

# === --- LLVM & Clang ----------------------------------------------------- ===

cd ${LLVM_WORK_ROOT}

rm -rf build-**
rm -rf ${LLVM_INSTALL_DIR}

mkdir ${LLVM_BUILD_DIR}
cd ${LLVM_BUILD_DIR}

cmake -G "${LLVM_GENERATOR}"                                       \
    -DCMAKE_BUILD_TYPE=Release                                     \
    -DCMAKE_INSTALL_PREFIX=${LLVM_INSTALL_DIR}                     \
    -DLLVM_ENABLE_PROJECTS='clang;lld;compiler-rt'                 \
    -DLLVM_ENABLE_ASSERTIONS=NO                                    \
    -DLLVM_ENABLE_THREADS=NO                                       \
    -DLLVM_TARGETS_TO_BUILD='${SINGLE_ARCH}'                       \
    ${LLVM_CMAKE_MACOS_FLAGS}                                      \
    ${LLVM_CMAKE_LD_FLAGS}                                         \
    ../src/llvm/

time ninja
cmake --install .

# === --- Libcxx ----------------------------------------------------------- ===

export PATH=${LLVM_INSTALL_DIR}/bin:$PATH

cd ${LLVM_WORK_ROOT}

mkdir ${LIBCXX_BUILD_DIR}
cd ${LIBCXX_BUILD_DIR}

cmake -G "${LLVM_GENERATOR}"                                      \
    -DCMAKE_BUILD_TYPE=Release                                    \
    -DCMAKE_INSTALL_PREFIX=${LLVM_INSTALL_DIR}                    \
    -DCMAKE_CXX_COMPILER=clang++                                  \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind"           \
    ../src/runtimes

ninja
cmake --install .

# === --- Clean Up --------------------------------------------------------- ===
#
cd ${LLVM_WORK_ROOT}
rm -rf build-**
