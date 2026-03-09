# vim:

FMT_FOLDERS = cmd include src

CFLAGS += -std=c99 -O2
# CFLAGS += -std=c99 -O2 -Wall -Werror -pedantic -Wno-c11-extensions
CFLAGS += -I.

SRC = $(wildcard src/*.c)

# -------
# actions
#
default: prompt sync_git_repo

prompt:
	mkdir -p bin && ${CC} $(CFLAGS) -o bin/$@ $(SRC) cmd/$@/main.c

sync_git_repo:
	mkdir -p bin && ${CC} $(CFLAGS) -o bin/$@ $(SRC) cmd/$@/main.c

# -----------
# maintenance
#
fmt:
	~/Workspace/y/tools/scripts/clang_format_all.sh $(FMT_FOLDERS)

test: default

clean:
	rm -rf bin

