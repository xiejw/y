#!/usr/bin/env bash
set -euo pipefail

#
# Usage: ./install_go.sh [build-dir]
#
# Installs the Go toolchain into <build-dir>/usr/go/ and symlinks
# go/gofmt into <build-dir>/usr/bin/. Skips if already installed.
# Run this first to bootstrap kit before using `go run .`.
# Defaults to ~/Workspace/build if no build-dir is given.
#

GO_VERSION="1.26.2"

BUILD_DIR="${1:-$HOME/Workspace/build}"
PREFIX="$BUILD_DIR/usr"
WORK_DIR="$BUILD_DIR/go"

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

echo "[config]  go version: $GO_VERSION"
echo "[config]  tarball:    $TARBALL"
echo "[config]  url:        $DOWNLOAD_URL"
echo "[config]  build dir:  $BUILD_DIR"
echo "[config]  prefix:     $PREFIX"
echo "[config]  work dir:   $WORK_DIR"
echo "[config]  go bin:     $BIN_GO"

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
