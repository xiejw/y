# vim:

FMT_FOLDERS     = cmd c

CFLAGS         +=-std=c99 -O2 -Wall -Werror -pedantic -Wno-c11-extensions

# -------
# actions
#
default: prompt sync_git_repo du2tree nginxfmt

prompt:
	mkdir -p bin && ${CC} ${CFLAGS} -I. -o bin/$@ cmd/$@/main.c

sync_git_repo:
	mkdir -p bin && ${CC} ${CFLAGS} -I. -o bin/$@ cmd/$@/main.c

du2tree:
	mkdir -p bin && ${CC} ${CFLAGS} -I. -o bin/$@ cmd/$@/main.c

nginxfmt:
	mkdir -p bin && ${CC} ${CFLAGS} -I. -o bin/$@ cmd/$@/main.c

# -----------
# maintenance
#
fmt:
	~/Workspace/y/tools/clang_format_all.sh ${FMT_FOLDERS}

clean:
	rm -rf bin

