#!/bin/sh
#
# https://www.tug.org/texlive/quickinstall.html
#   wget https://mirror.ctan.org/systems/texlive/tlnet/install-tl-unx.tar.gz
#
# scheme
#   basic https://tug.org/svn/texlive/trunk/Master/tlpkg/tlpsrc/scheme-basic.tlpsrc?view=markup
#   full  https://tug.org/svn/texlive/trunk/Master/tlpkg/tlpsrc/scheme-full.tlpsrc?view=markup
#
# remember to modify PATH
#
# latex packages:
#   tlmgr install listings xcolor enumitem
#
# metapost:
#   tlmgr install metapost
#
# fonts:
#   tlmgr search --file phvr7t --global
#   tlmgr install helvetic

TEXDIR=~/Workspace/build/texlive/`date +"%Y"`
#SCHEME=basic
SCHEME=full

mkdir -p ${TEXDIR}

./install-tl \
  -no-gui \
  -no-doc-install \
  -no-src-install \
  -paper=letter \
  -portable \
  -scheme=${SCHEME} \
  -texdir=${TEXDIR}
