BUILD        = .build

CXXFLAGS    += -std=c++20 -Wall -Werror -pedantic -Wextra


run: compile_test
	${BUILD}/test

compile: compile_test

compile_test: ${BUILD}/test

${BUILD}/test: cmd/test/main.cc | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ $<

${BUILD}:
	mkdir -p ${BUILD}

test: compile

fmt:
	~/Workspace/y/tools/clang_format_all.sh cmd

clean:
	rm -rf ${BUILD}
