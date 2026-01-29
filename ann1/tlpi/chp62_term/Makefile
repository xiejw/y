BUILD    = .build

CFLAGS  += -std=c23 -O2 -DNDEBUG
CFLAGS  += -Wall -pedantic -Wextra -Wfatal-errors -Wconversion
CFLAGS  += -I.

run: compile
	${BUILD}/tty

compile: ${BUILD}/tty

${BUILD}/tty: | ${BUILD}
	${CC} ${CFLAGS} -o ${BUILD}/tty main.c tty.c

${BUILD}:
	mkdir -p ${BUILD}

clean:
	rm -rf ${BUILD}

test: compile

release: compile

fmt:
	~/Workspace/y/tools/clang_format_all.sh .


