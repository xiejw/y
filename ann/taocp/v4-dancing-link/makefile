CMD              =  cmd
SRC              =  src
INC              =  include

EVE_FMT_FOLDERS  = ${CMD} ${SRC} ${INC}
EVE_PATH         = ../../eve
include ${EVE_PATH}/eve.mk

CXXFLAGS        += -Iinclude
LDFLAGS         += ${EVE_LIB}

# ------------------------------------------------------------------------------
# Libs
#
MODULES          = ${BUILD_OBJ}/algos_dal.o

# ------------------------------------------------------------------------------
# Actions
#

.DEFAULT_GOAL   = compile

# incrementally compile all libraries.
compile: ${MODULES} ${EVE_LIB}

run: compile
	${BUILD}/sudoku_main

release:
	@echo compile with RELEASE mode
	${MK} RELEASE=1 compile || exit 1

test: compile run

# ------------------------------------------------------------------------------
# Rules for sub-modules
#
${BUILD_OBJ}/algos_%.o: ${SRC}/algos/%.cc ${INC}/algos/*.h | ${BUILD_OBJ}
	${CXX} -o $@ ${CXXFLAGS} -c $<

# ------------------------------------------------------------------------------
# Cmds
#
compile: ${BUILD}/sudoku_main

${BUILD}/sudoku_main: ${BUILD}/sudoku_main.o ${MODULES} | ${BUILD} ${EVE_LIB}
	${CXX} -o $@ $^ ${LDFLAGS}

${BUILD}/%_main.o: cmd/%/main.cc | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ -c $^

# ------------------------------------------------------------------------------
# Tests
#

TEST_LIBS       = ${BUILD_OBJ}/algos_dal_test.o
