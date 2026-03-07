# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is a collection of standalone utility scripts (shell, Go) for system administration tasks:
- kernel compilation/installation,
- LLVM builds,
- Go installation,
- certificate renewal,
- nginx config formatting, etc.

## Commands

- **Format Go files**: `make fmt` (runs `gofmt -w -l *.go`)

## Conventions

- Scripts are standalone — no shared libraries or build system beyond the simple makefile
- Go files (e.g. `nginxfmt.go`, `delete_unused_dirs.go`) are meant to be run directly with `go run`
