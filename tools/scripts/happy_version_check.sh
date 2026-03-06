#!/bin/bash
#
# This script tries to check the Latest CLI and compare with local installed
# version.
set -e

CLI_NAME=happy
REPO=x/ann

find_latest() {
  VERSION=`make --no-print-directory -C ~/Workspace/${REPO}/${CLI_NAME} version`

  echo "Find latest ${CLI_NAME} version ${VERSION}"
}

find_existing_version() {
  LOCAL_VERSION=`~/Workspace/.bin/${CLI_NAME} --version |
    grep -E -o '[0-9\.]+'`
  echo "Find local  ${CLI_NAME} version ${LOCAL_VERSION}"
}

find_latest
find_existing_version

RED='\033[0;31m'
NC='\033[0m' # No Color

if [ "${VERSION}" == "${LOCAL_VERSION}" ]; then
  echo "Version same for ${CLI_NAME}"
else
  echo -e "${RED}!!! Upgrade ${CLI_NAME} !!!${NC}"
  make -C ~/Workspace/${REPO}/${CLI_NAME} install
fi
