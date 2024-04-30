#!/bin/bash
#
# This script tries to send an email to xieqi family. It simply forwards all
# arguments to the mockingjay client.
#
# In addition, the script also tries to compile the client, if absent, in
# release mode.
set -e
set -x

MOCKINGJAY_HOME=~/Workspace/x/ann/mockingjay
MOCKINGJAY_CLIENT=${MOCKINGJAY_HOME}/.build/client

if ! test -f ${MOCKINGJAY_CLIENT}; then
  echo "mockingjay client not exist"
  echo "building"
  make -C ${MOCKINGJAY_HOME} release
fi

${MOCKINGJAY_CLIENT} "$@"
