#!/bin/bash
#

set -e

if [ "$(which clang-format)" == "" ]; then
    echo "clang-format cannot be found"
    exit 1
fi

export CLANG_FMT_EXT="-type f -iname *.h -o -iname *.c -o -name *.cc"
export CLANG_FMT_CLI="clang-format -i --style=file"
sh -c 'find "$@" ${CLANG_FMT_EXT} | xargs ${CLANG_FMT_CLI}' sh "$@"
