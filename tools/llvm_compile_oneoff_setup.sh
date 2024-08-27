#!/bin/bash
#
# This script sets up the llvm project source code one off for xieqi machine
# which does not have git set up due to limited space.

set -x
set -e

cd ~/Workspace/build/llvm
git clone --depth 1 https://github.com/llvm/llvm-project.git
rm -rf llvm-project/.git
ln -sf llvm-project/ src
