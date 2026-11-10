# CLAUDE.md — bedrock

## Overview

**macOS and Linux.** This Go program installs developer tools from source without a system package manager (Homebrew, apt, …). Supported platforms are `arm64-darwin` (Apple Silicon macOS) and `amd64-linux`; other platforms are not supported.

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
  <tool>/      ← per-tool build workspace (one directory per tool)
    <tarball>
    <extracted-source>/
```

Each tool gets its own subdirectory under `~/Workspace/build/` for source and tarballs. After a successful install the whole `<tool>/` directory is deleted, unless `--keep_compiled_assets` is passed (then nothing is deleted). On failure it is left in place for debugging.

## Prerequisites

Go must already be installed and on `PATH` to run bedrock itself. Rust is not
needed: the Rust-based tools (fd, eza, ripgrep, zoxide) are downloaded as
prebuilt binaries from this repo's GitHub releases (`BinaryDownload`), keyed
by `<GOARCH>-<GOOS>`: `arm64-darwin` and `amd64-linux`. Other platforms error
out.

## Usage

```sh
# Install all missing tools (skips already-installed ones)
go run ./cmd

# Install a single tool by name
go run ./cmd <name>

# Install a single tool to a custom prefix (overrides ~/Workspace/build)
go run ./cmd <name> <build-dir>

# Keep tarballs, sources and build outputs (flags go before positional args)
go run ./cmd --keep_compiled_assets [<name> [<build-dir>]]
```

Examples:

```sh
go run ./cmd vim
go run ./cmd git /tmp/build
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
go run ./cmd bash
sudo vim /etc/shells
chsh
```

## Key source files

- `cmd/main.go` — tool list and entry point (package `main`)
- `src/installer.go` — package `bedrock/src`: `Env`, `Tool`, all installer constructors (`CAutoconf`, `CMakeBuild`, `GoInstall`, `BinaryDownload`) and `InstallToolsWithFilter`

## Verification

After any edit, confirm the program compiles:

```sh
go build ./...
```
