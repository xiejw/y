#!/bin/bash
#
# This script tries to check the Latest Jarvis (jv) and compare with local
# installed version.
set -e

find_latest() {
  VERSION=`make --no-print-directory -C ~/Workspace/x/ann/jarvis version`

  echo "Find latest Jarvis version ${VERSION}"
}

find_existing_version() {
  LOCAL_VERSION=`~/Workspace/.bin/jarvis --version |
    grep -E -o '[0-9\.]+'`
  echo "Find local  Jarvis version ${LOCAL_VERSION}"
}

find_latest
find_existing_version

RED='\033[0;31m'
NC='\033[0m' # No Color

if [ "${VERSION}" == "${LOCAL_VERSION}" ]; then
  echo "Version same for Jarvis"
else
  echo -e "${RED}!!! Upgrade Jarvis !!!${NC}"
  make -C ~/Workspace/x/ann/jarvis install
fi
