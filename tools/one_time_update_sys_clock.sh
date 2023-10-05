#!/bin/sh
#
sudo date -s @`telnet xieqi.org  8771 2> /dev/null | tail -n 1 | sed 's/^ *//g'`
sudo hwclock --systohc
