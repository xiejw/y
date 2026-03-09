# vim:

FMT_FOLDERS = cmd include src

CFLAGS += -std=c99 -O2
# CFLAGS += -std=c99 -O2 -Wall -Werror -pedantic -Wno-c11-extensions

SRC = src/exec.c src/file_reader.c src/git.c src/path.c src/read_config_file.c

# -------
# actions
#
default: prompt sync_git_repo

prompt:
	mkdir -p bin && clang $(CFLAGS) -Iinclude -o bin/$@ $(SRC) cmd/$@/main.c

sync_git_repo:
	mkdir -p bin && clang $(CFLAGS) -Iinclude -o bin/$@ $(SRC) cmd/$@/main.c

# -----------
# maintenance
#
fmt:
	~/Workspace/y/tools/scripts/clang_format_all.sh $(FMT_FOLDERS)

test: default

clean:
	rm -rf bin

