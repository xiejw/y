#!/bin/sh
#
# intended to be run on debian without GPU support.

#
# prepare cmake
#

. ${HOME}/Workspace/build/pyenv/bin/activate &&               \
  which python3 &&                                            \
  pip3 install pyyaml typing_extensions &&                    \
  cmake                                                       \
    -G Ninja                                                  \
    -DBUILD_PYTHON=OFF                                        \
    -DBUILD_SHARED_LIBS=ON                                    \
    -DCMAKE_BUILD_TYPE:STRING=Release                         \
    -DPYTHON_EXECUTABLE:PATH=`which python3`                  \
    -DCMAKE_INSTALL_PREFIX:PATH=../install-`date +'%Y-%m-%d'` \
    ../src

#
# remove clang flags to avoid compilation error.
#
# As of 2023-08, caffe2 has some files failing this clang flag. This patch
# basically removes all of them.
sed -i -r 's/-Wno-missing-prototypes//g' build.ninja
sed -i -r 's/-Wmissing-prototypes//g' build.ninja
sed -i -r 's/-Wno-error=missing-prototypes//g' build.ninja
sed -i -r 's/-Werror=missing-prototypes//g' build.ninja
