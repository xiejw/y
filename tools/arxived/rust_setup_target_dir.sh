#!/bin/bash
#
# Set up the target dir for rust project.
#
# Rust puts all compilation artifacts into `target` folder. But my project uses
# `.build`, which has some nice properties, e.g., ignored by fd and vim.
#
# See
# - https://github.com/xiejw/y/blob/main/doc/rust.md

set -x
set -e

mkdir .cargo
echo "[build]
target-dir = \".build\"" >> .cargo/config.toml
