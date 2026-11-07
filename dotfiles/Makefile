# vim:

FMT_FOLDERS = cmd include src

CFLAGS += -std=c99 -O2
# CFLAGS += -std=c99 -O2 -Wall -Werror -pedantic -Wno-c11-extensions
CFLAGS += -I.

SRC = $(wildcard src/*.c)

BIN_DIR ?= $(HOME)/.local/bin

.PHONY: default prompt sync_git_repo fmt test clean

# -------
# actions
#
default: prompt sync_git_repo

prompt:
	mkdir -p $(BIN_DIR) && ${CC} $(CFLAGS) -o $(BIN_DIR)/$@ $(SRC) cmd/$@/main.c

sync_git_repo:
	mkdir -p $(BIN_DIR) && go build -o $(BIN_DIR)/$@ ./$@/cmd/main.go

# -----------
# maintenance
#
fmt:
	~/Workspace/y/tools/scripts/clang_format_all.sh $(FMT_FOLDERS)
	gofmt -w sync_git_repo

test: default

clean:
	rm -f $(BIN_DIR)/prompt $(BIN_DIR)/sync_git_repo

