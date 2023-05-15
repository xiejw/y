#!/bin/sh

. ../pyenv/bin/activate && \
  which python3 &&  \
  pip3 install pyyaml typing_extensions && \
  cmake \
    -G Ninja \
    -DUSE_MPS=ON \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DPYTHON_EXECUTABLE:PATH=`which python3` \
    -DCMAKE_INSTALL_PREFIX:PATH=../install \
    ../src
