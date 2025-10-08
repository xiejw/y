#!/bin/bash
#
# This script tries to upgrade the golang for macOS/arm64. It checks the
# current released version and then compare the local version. If upgrading is
# needed.
set -e

find_latest() {
  if [ -z "${VERSION}" ]; then
    VERSION=`curl --silent https://go.dev/dl/?mode=json |
      grep -v -E 'go[0-9\.]+(beta|rc)' |
      grep -E -o 'go[0-9\.]+' |
      grep -E -o '[0-9]\.[0-9]+(\.[0-9]+)?' |
      sort -V | uniq | tail -1`
  fi
  echo "Find latest Go version ${VERSION}"
}

find_existing_version() {
  GO_VERSION=`go version |
    grep -E -o 'go[0-9\.]+' |
    grep -E -o '[0-9\.]+'`
  echo "Find local  Go version ${GO_VERSION}"
}

find_arch() {
  ARCH=arm64
  PROC=`uname -p`
  if [ "${PROC}" == "i386" ]; then
    echo "Arch x86-64 is out of date for Go. Skip..."
    exit 0
  fi
  echo "Find Arch: ${ARCH}"
}

find_latest
find_existing_version

if [ "${VERSION}" == "${GO_VERSION}" ]; then
  echo "Version same. Skip..."
  exit 0
fi

find_arch

URL="https://go.dev/dl/go${VERSION}.darwin-${ARCH}.pkg"
curl -L -o /tmp/go.pkg "${URL}"
open -W /tmp/go.pkg
rm -rf /tmp/go.pkg
