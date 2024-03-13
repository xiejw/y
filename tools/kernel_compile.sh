#!/bin/bash
#
# This script tries to
# - download the linux kernel,
# - set up all the folders and config files and then
# - compile the kernel.
#
# Due to long compilation, the install is in a different file as sudo
# will time out most of time.
set -e

if [ -z "${VERSION}" ]; then
  echo "Please set VERSION like 6.7"
  exit 1
fi

rm linux*.tar.xz
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
