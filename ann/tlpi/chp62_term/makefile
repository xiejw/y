BUILD         = .build

CXXFLAGS     += -std=c++20
CXXFLAGS     += -Wall -pedantic -Wextra -Wfatal-errors -Wconversion
LDFLAGS      +=

compile: | ${BUILD}
	${CXX} ${CXXFLAGS} -o ${BUILD}/main tty.cc main.cc ${LDFLAGS}

${BUILD}:
	mkdir -p $@

run: compile
	${BUILD}/main

fmt:
	~/Workspace/y/tools/clang_format_all.sh .
