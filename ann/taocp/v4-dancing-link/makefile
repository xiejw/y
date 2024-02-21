BUILD         = .build
BUILD_OBJ     = ${BUILD}/objs

CXXFLAGS     += -std=c++20 -Wall -Werror -pedantic -Wextra
CXXFLAGS     += -Iinclude

# TODO
CXXFLAGS     += -I../../eva/include
LDFLAGS      += ../../eva/.build_release/libeva.a

CMD           =  cmd
SRC           =  src
INC           =  include

FMT           = ~/Workspace/y/tools/clang_format_all.sh

# ------------------------------------------------------------------------------
# libs
# ------------------------------------------------------------------------------
MODULES       = ${BUILD_OBJ}/algos_dal.o

# ------------------------------------------------------------------------------
# actions
# ------------------------------------------------------------------------------

.DEFAULT_GOAL   = compile

${BUILD}:
	@mkdir -p $@

${BUILD_OBJ}: | ${BUILD}
	@mkdir -p $@

# incrementally compile all libraries.
compile: ${MODULES}

# Maintenance actions
fmt:
	${FMT} ${CMD} ${SRC} ${INC}
clean:
	rm -rf ${BUILD} ${BUILD_OBJ}

# ------------------------------------------------------------------------------
# rules for sub-modules
# ------------------------------------------------------------------------------
${BUILD_OBJ}/algos_%.o: ${SRC}/algos/%.cc ${INC}/algos/*.h | ${BUILD_OBJ}
	${CXX} -o $@ ${CXXFLAGS} -c $<

# ------------------------------------------------------------------------------
# cmds
# ------------------------------------------------------------------------------

${BUILD}/%_main.o: cmd/%/main.cc | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ -c $^

${BUILD}/sudoku_main: ${BUILD}/sudoku_main.o ${MODULES} | ${BUILD}
	${CXX} -o $@ $^ ${LDFLAGS}

run: compile
	${BUILD}/sudoku_main

compile: ${BUILD}/sudoku_main

# ------------------------------------------------------------------------------
# tests
# ------------------------------------------------------------------------------

# unit tests
TEST_LIBS       = ${BUILD_OBJ}/algos_dal_test.o


