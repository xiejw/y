#!/bin/sh
#
# vim: ft=bash

# set -x

DATE=`date +"%Y-%m-%d"`
TMPFILE=$(mktemp /tmp/${DATE}_checksum.txt.XXXXXX)
find -L . -type f -not -path '*/.*' | sort | sed -e "s/^.\///" | xargs -I {} shasum -a 256 "{}" | tee ${TMPFILE}
echo "writen to ${TMPFILE}" 1>&2
