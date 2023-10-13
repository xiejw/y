# vim:

FMT_FOLDERS     = cmd c

CLANG_EXTS      = -iname *.h -o -iname *.c
CLANG_FMT       = clang-format -i --style=file

CFLAGS         +=-std=c99 -O2 -Wall -Werror -pedantic -Wno-c11-extensions

#
# actions
#
default: prompt sync_git_repo

prompt:
	mkdir -p bin && ${CC} ${CFLAGS} -I. -o bin/$@ cmd/$@/main.c

sync_git_repo:
	mkdir -p bin && ${CC} ${CFLAGS} -I. -o bin/$@ cmd/$@/main.c

#
# maintenance
#
fmt:
	find ${FMT_FOLDERS} ${CLANG_EXTS} | xargs ${CLANG_FMT}

clean:
	rm -rf bin

