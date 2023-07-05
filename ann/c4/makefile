BUILD=.build

.PHONY: run compile

run: compile
	${BUILD}/c4

fmt:
	clang-format -i --style=file main.c


${BUILD}/c4: main.c | ${BUILD}
	cc -o $@ $<

${BUILD}:
	mkdir -p ${BUILD}

compile: ${BUILD}/c4

