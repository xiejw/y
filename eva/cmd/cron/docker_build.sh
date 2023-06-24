#!/bin/sh

set -e
set -x

# DOCKFILE, BUILD_DIR and DOCKER_DIR are coming from env vars.

MK_FLAGS="RELEASE=1 V=1"
export CFLAGS=-static

make ${MK_FLAGS} clean compile ${BUILD_DIR}/cron
mkdir -p ${DOCKER_DIR}
cp ${BUILD_DIR}/cron ${DOCKER_DIR}
docker build -t xiejw/cron -f ${DOCKFILE} ${DOCKER_DIR}
