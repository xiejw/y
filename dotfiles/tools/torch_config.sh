#!/bin/sh

#
# check clang
#
CLANG=`which clang`
CLANG_EXPECTED="/usr/bin/clang"

if [ "$CLANG" == "$CLANG_EXPECTED" ]; then
    echo "clang is correct in PATH: $CLANG"
else
    echo "clang is not stock version: $CLANG"
    echo "panic: libtorch must be compiled with stock version: $CLANG_EXPECTED"
    exit 1
fi

#
# prepare cmake
#

. ../pyenv/bin/activate && \
  which python3 &&  \
  pip3 install pyyaml typing_extensions && \
  cmake \
    -G Ninja \
    -DUSE_MPS=ON \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DPYTHON_EXECUTABLE:PATH=`which python3` \
    -DCMAKE_INSTALL_PREFIX:PATH=../install-`date +'%Y-%m-%d'` \
    ../src
