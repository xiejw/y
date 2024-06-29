BUILD     = .build
LDFLAGS  +=

compile: | ${BUILD}
	${CC} -std=c11 -o ${BUILD}/main tty.c main.c ${LDFLAGS}

${BUILD}:
	mkdir -p $@

run: compile
	${BUILD}/main

fmt:
	~/Workspace/y/tools/clang_format_all.sh .
