#!/bin/bash
#
# This script compiles libcxx
#
# Bash profile set up
#
#     libcxx (){
#         export LIBCXX_PATH=~/Workspace/build/llvm/install-libc++
#         export CXX="clang++ -nostdinc++ -nostdlib++ -isystem ${LIBCXX_PATH}/include/c++/v1 -L ${LIBCXX_PATH}/lib -Wl,-rpath,${LIBCXX_PATH}/lib -lc++"
#         export CXXFLAGS=-Wno-unused-command-line-argument
#     }
#
#     libcxx
#
# Refs:
#
#     https://libcxx.llvm.org/BuildingLibcxx.html
#     https://libcxx.llvm.org/UsingLibcxx.html
#     https://libcxx.llvm.org/Modules.html

set -x
set -e

TODAY_STR=`date '+%Y-%m-%d'`
LIBCXX_BUILD_DIR=libc++-build-${TODAY_STR}

mkdir ${LIBCXX_BUILD_DIR}
cd ${LIBCXX_BUILD_DIR}
cmake -G Ninja -S ../src/runtimes -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind"
ninja
cd ..
ln -sf ${LIBCXX_BUILD_DIR} install-libc++

