#!/usr/bin/env bash
set -euo pipefail

#
# Usage: ./install_go.sh
#
# Installs the Go toolchain into ~/Workspace/build/usr/go/ and symlinks
# go/gofmt into ~/Workspace/build/usr/bin/. Skips if already installed.
# Run this first to bootstrap kit before using `go run .`.
#

GO_VERSION="1.26.1"

PREFIX="$HOME/Workspace/build/usr"
WORK_DIR="$HOME/Workspace/build/go"

# Detect OS and arch
OS="$(uname -s | tr '[:upper:]' '[:lower:]')"
ARCH="$(uname -m)"
case "$ARCH" in
  aarch64) ARCH="arm64" ;;
  x86_64)  ARCH="amd64" ;;
esac

TARBALL="go${GO_VERSION}.${OS}-${ARCH}.tar.gz"
TARBALL_PATH="$WORK_DIR/$TARBALL"
DOWNLOAD_URL="https://go.dev/dl/$TARBALL"

BIN_GO="$PREFIX/bin/go"

# Skip if already installed
if [ -e "$BIN_GO" ]; then
  echo "[skip]    go (already in $BIN_GO)"
  exit 0
fi

echo "[install] go"

mkdir -p "$WORK_DIR"
mkdir -p "$PREFIX/bin"

# Download tarball if not already present
if [ ! -f "$TARBALL_PATH" ]; then
  curl -fL "$DOWNLOAD_URL" -o "$TARBALL_PATH"
fi

# Extract into $PREFIX — produces $PREFIX/go/
tar -xzf "$TARBALL_PATH" -C "$PREFIX"

# Symlink binaries using relative paths
ln -sf "../go/bin/go"    "$PREFIX/bin/go"
ln -sf "../go/bin/gofmt" "$PREFIX/bin/gofmt"

echo "[done]    go"
