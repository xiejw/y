#!/bin/bash
set -e

if [ -z "${VERSION}" ]; then
  echo "Please set VERSION like 6.7"
  exit 1
fi

echo "========================================================================="
echo "delete files in /lib/modules"
echo "========================================================================="
cd /lib/modules
ls -l -d *${VERSION}*-xiejw* || echo "no files"*
sudo rm -rf *${VERSION}*-xiejw*

echo "========================================================================="
echo "delete files in /boot"
echo "========================================================================="
cd /boot
ls -l -d *${VERSION}*-xiejw* || echo "no files"
sudo rm -rf *${VERSION}*-xiejw*
