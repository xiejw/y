#!/bin/bash
#
# This script tries to send an email to xieqi family. It simply forwards all
# arguments to the mockingjay client.
#
# In addition, the script also tries to compile the client, if absent, in
# release mode.
set -e
set -x

MOCKINGJAY_HOME=${HOME}/Workspace/x/ann/mockingjay
MOCKINGJAY_CLIENT=${HOME}/Workspace/.bin/email

if ! test -f ${MOCKINGJAY_CLIENT}; then
  echo "mockingjay client not exist"
  echo "building"
  make -C ${MOCKINGJAY_HOME} release install
fi

${MOCKINGJAY_CLIENT} "$@"
