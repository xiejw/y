# CLAUDE.md — kit

## Overview

**macOS only.** This Go program installs developer tools from source without Homebrew. It is not intended for Linux or other platforms.

All artifacts are installed into a **fixed, hardcoded prefix**:

```
~/Workspace/build/usr/
```

This prefix is not configurable at the tool level — every installer targets it. The directory is created automatically if it does not exist.

## Install layout

```
~/Workspace/build/
  usr/
    bin/       ← compiled binaries (git, cmake, vim, …)
    lib/       ← libraries (libpcre2-8.dylib, …)
    go/        ← Go toolchain (go/gofmt symlinked into bin/)
    rust/      ← Rust toolchain (cargo/rustc symlinked into bin/)
  <tool>/      ← per-tool build workspace (one directory per tool)
    <tarball>
    <extracted-source>/
```

Each tool gets its own subdirectory under `~/Workspace/build/` for source and tarballs. Build artifacts are cleaned up after installation.

## Bootstrap

`kit` is written in Go, so Go must be available before running it.
Use `install_go.sh` to install the Go toolchain from a pre-built tarball:

    ./install_go.sh

This installs Go into `~/Workspace/build/usr/go/` and symlinks `go`/`gofmt`
into `~/Workspace/build/usr/bin/`. Once done, use `go run .` as normal.

Use `install_rs.sh` to install the Rust toolchain via rustup:

    ./install_rs.sh

This runs the official rustup installer (`curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`), which is required before cargo-based tools can be built.

## Usage

```sh
# Install all missing tools (skips already-installed ones)
go run .

# Install a single tool by name
go run . <name>

# Install a single tool to a custom prefix (overrides ~/Workspace/build)
go run . <name> <build-dir>
```

Examples:

```sh
go run . vim
go run . git /tmp/build
```

The program prints one line per tool:

```
[skip]    vim (already in /…/usr/bin)
[install] git
[done]    git
[error]   cmake: configure: exit status 1
```

## Special Note for Bash

```
go run . bash
sudo vim /etc/shells
chsh
```

## Key source files

- `main.go` — tool list and entry point
- `installer.go` — `Env`, `Tool`, and all installer constructors (`CAutoconf`, `CMakeBuild`, `GoInstall`, `RustCargoInstall`)

## Verification

After any edit, confirm the program compiles:

```sh
go build -o /dev/null .
```
