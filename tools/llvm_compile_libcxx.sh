#!/bin/bash
#
# This script compiles libcxx
#
# Bash profile set up
#
#     libcxx (){
#         export LIBCXX_PATH=~/Workspace/build/usr/libcxx/
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
LIBCXX_BUILD_DIR=build-libcxx-${TODAY_STR}

LIBCXX_INSTALL_DIR=${HOME}/Workspace/build/usr/libcxx
LIBCXX_INSTALL_DIR_TMP=${HOME}/Workspace/build/usr/libcxx_${TODAY_STR}

mkdir ${LIBCXX_BUILD_DIR}
cd ${LIBCXX_BUILD_DIR}
cmake -G Ninja -S ../src/runtimes                             \
    -DCMAKE_CXX_COMPILER=clang++                              \
    -DCMAKE_INSTALL_PREFIX=${LIBCXX_INSTALL_DIR_TMP}          \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind"
ninja
rm -rf ${LIBCXX_INSTALL_DIR}*
cmake --install .
ln -sf ${LIBCXX_INSTALL_DIR_TMP} ${LIBCXX_INSTALL_DIR}


