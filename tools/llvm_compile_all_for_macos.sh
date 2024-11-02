#!/bin/bash
#
# This script compiles llvm and libcxx for old macos.
#
# This script leverages all existing tools.
#

set -x
set -e

BUILD_HOME=$HOME/Workspace/build

unset CXX
~/Workspace/.bin/jarvis llvm-compile
export PATH=$BUILD_HOME/llvm/install/bin:$PATH
export CXX=clang++
~/Workspace/.bin/jarvis libcxx-compile
~/Workspace/.bin/email 'macos llvm'

