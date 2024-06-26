#!/bin/bash
#
# This script tries to check the Latest Linux Kernel and compare with local
# kernel version.
set -e

find_latest() {
  if [ -z "${VERSION}" ]; then
    VERSION=`curl -s https://www.kernel.org |
      grep -A1 latest_link |
      tail -n1 |
      grep -o -E '>[^<]+' |
      grep -o -E '[^>]+'`
  fi

  # Official site reports .0 version as X.Y rather than X.Y.0. So here, convert
  # X.Y to X.Y.0 as the uname -r reports that format.
  if [[ "${VERSION}" =~ ^[0-9]+\.[0-9]+$ ]]; then
    VERSION="${VERSION}.0"
  fi

  echo "Find latest Kernel version ${VERSION}"
}

find_existing_version() {
  LOCAL_VERSION=`uname -r |
    grep -E -o '[0-9\.]+'`
  echo "Find local  Kernel version ${LOCAL_VERSION}"
}

find_latest
find_existing_version

RED='\033[0;31m'
NC='\033[0m' # No Color

if [ "${VERSION}" == "${LOCAL_VERSION}" ]; then
  echo "Version same for Linux Kernel"
else
  echo -e "${RED}!!! Consider to upgrade Linux Kernel !!!${NC}"
fi
