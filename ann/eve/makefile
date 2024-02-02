BUILD        = .build
BUILD_OBJ    = ${BUILD}/objs

CXXFLAGS    += -std=c++20 -Wall -Werror -pedantic -Wextra
CXXFLAGS    += -Iinclude

FMT          = ~/Workspace/y/tools/clang_format_all.sh
AR           = ar -cr

MODULES     += ${BUILD_OBJ}/testing_testing.o

# ==============================================================================
# Top Level actions

release: ${BUILD}/libeve.a

test: compile

# Maintenance actions
fmt:
	${FMT} cmd include src

clean:
	rm -rf ${BUILD} ${BUILD_OBJ}


# ==============================================================================
# Project actions

.DEFAULT_GOAL = run

run: compile_test
	${BUILD}/test_main


compile: compile_test

compile_test: ${BUILD}/test_main

# ==============================================================================
# Objects actions
#
# see https://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.NOTINTERMEDIATE:

${BUILD}:
	mkdir -p ${BUILD}

${BUILD_OBJ}:
	mkdir -p ${BUILD_OBJ}

${BUILD}/%_main: ${BUILD}/%_main.o ${BUILD}/libeve.a | ${BUILD}
	${CXX} -o $@ $^

${BUILD}/%_main.o: cmd/%/main.cc | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ -c $^

${BUILD}/libeve.a: ${MODULES} | ${BUILD}
	${AR} $@ $^

${BUILD_OBJ}/testing_%.o: src/testing/%.cc | ${BUILD_OBJ}
	${CXX} ${CXXFLAGS} -o $@ -c $^

