#!/bin/sh

set -x

DATE=`date +"%Y-%m-%d"`
find -L . -type f -not -path '*/.*' | sort | sed -e "s/^.\///" | xargs -I {} shasum -a 256 "{}" | tee ${DATE}_checksum.txt
