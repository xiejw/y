#!/bin/bash
#
# This script tries to
# - download the linux kernel,
# - set up all the folders and config files and then
# - compile the kernel.
#
# NOTE:
# - Please run this with normal user.
# - Due to long compilation, the install is in a different file as sudo will
#   time out most of time. This means `modules_install` is not part of this
#   file.
# - All source code trees are renamed in linux-*-src so the deletion could be
#   easier.
#
#
# TODO:
# - The final make cmd is single thread now. We could detect this with
#
#     cat /proc/cpuinfo | grep "processor" | wc -l
#
set -e

if [ -z "${VERSION}" ]; then
  echo "Please set VERSION like 6.7"
  exit 1
fi

cd ~/Workspace/kernel
rm -rf linux*.tar.xz
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-${VERSION}.tar.xz
tar xvf linux-${VERSION}.tar.xz
rm -rf linux-*-src
rm -rf linux
mv linux-${VERSION} linux-${VERSION}-src
ln -sf linux-${VERSION}-src linux
cp *-config linux/.config
mv *-config configs
cd linux
make olddefconfig
cp .config ../linux-${VERSION}-config
time make -j1
