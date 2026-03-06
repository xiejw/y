#!/bin/bash
#
set -x
set -e

LLVM_DIR=${HOME}/Workspace/build/usr/llvm
LLVM_INSTALL_DIR=${HOME}/Workspace/build/usr/llvm2

rm -rf ${LLVM_DIR}
mv ${LLVM_INSTALL_DIR} ${LLVM_DIR}
