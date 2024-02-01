BUILD        = .build

CXXFLAGS    += -std=c++20 -Wall -Werror -pedantic -Wextra
CXXFLAGS    += -Iinclude

FMT          = ~/Workspace/y/tools/clang_format_all.sh


run: compile_test
	${BUILD}/test

compile: compile_test

compile_test: ${BUILD}/test

${BUILD}/test: cmd/test/main.cc src/testing/testing.cc | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ $^

${BUILD}:
	mkdir -p ${BUILD}

test: compile

fmt:
	${FMT} cmd include src

clean:
	rm -rf ${BUILD}
