BUILD         = .build
BUILD_OBJ     = ${BUILD}/objs

CXXFLAGS     += -std=c++20 -Wall -Werror -pedantic -Wextra -Wfatal-errors
CXXFLAGS     += -Iinclude

CMD           =  cmd
SRC           =  src
INC           =  include

CXXFLAGS     += -I../../eve/include
LDFLAGS      += ../../eve/.build_release/libeve.a

FMT           = ~/Workspace/y/tools/clang_format_all.sh

ifdef RELEASE
CXXFLAGS     += -DNDEBUG -O2
endif

ifdef ASAN
# Ref: https://clang.llvm.org/docs/AddressSanitizer.html
CXXFLAGS         += -fsanitize=address -g
LDFLAGS          += -fsanitize=address -g
endif  # ASAN

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

release:
	@echo compile with RELEASE mode
	make RELEASE=1 compile || exit 1

test: compile run

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


