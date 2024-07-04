CMD              =  cmd
SRC              =  src
INC              =  include

EVE_FMT_FOLDERS  = ${CMD} ${SRC} ${INC}
EVE_PATH         = ../../eve
include ${EVE_PATH}/eve.mk

CXXFLAGS        += -I${EVE_PATH}/include
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
compile: ${MODULES}

run: compile
	${BUILD}/sudoku_main

release:
	@echo compile with RELEASE mode
	make RELEASE=1 compile || exit 1

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

${BUILD}/sudoku_main: ${BUILD}/sudoku_main.o ${MODULES} ${EVE_LIB} | ${BUILD}
	${CXX} -o $@ $^ ${LDFLAGS}

${BUILD}/%_main.o: cmd/%/main.cc | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ -c $^

# ------------------------------------------------------------------------------
# Tests
#

TEST_LIBS       = ${BUILD_OBJ}/algos_dal_test.o
