#!/bin/bash
#
# The script will find all cc/c files under `folder_path` and format them with
# clang-format.
#
# Usage: <script> <folder_path>

set -e

if [ "$(which clang-format)" == "" ]; then
    echo "clang-format cannot be found"
    exit 1
fi

export CLANG_FMT_CLI="clang-format -i --style=file"
fd . "$@" -t f -e h -e c -e cc -e ccm -e cu -x ${CLANG_FMT_CLI} {}
