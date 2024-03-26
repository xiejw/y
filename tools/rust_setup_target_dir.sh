#!/bin/bash
#
# Set up the target dir for rust project. See
# - https://github.com/xiejw/y/blob/main/doc/rust.md

set -x
set -e

mkdir .cargo
echo "[build]
target-dir = \".build\"" >> .cargo/config
