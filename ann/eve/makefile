BUILD         = .build
BUILD_OBJ     = ${BUILD}/objs

CXXFLAGS     += -std=c++20 -Wall -pedantic -Wextra
ifndef SKIP_WERROR
CXXFLAGS     += -Werror
endif
CXXFLAGS     += -Iinclude

FMT           = ~/Workspace/y/tools/clang_format_all.sh
AR            = ar -cr

MODULES      += ${BUILD_OBJ}/adt_sds.o
MODULES      += ${BUILD_OBJ}/base_error.o
MODULES      += ${BUILD_OBJ}/base_log.o

TEST_MODULES += ${BUILD_OBJ}/testing_testing.o
TEST_MODULES +=  ${BUILD_OBJ}/adt_sds_test.o
TEST_MODULES +=  ${BUILD_OBJ}/base_error_test.o

ifdef ASAN
# Ref: https://clang.llvm.org/docs/AddressSanitizer.html
CXXFLAGS     += -fsanitize=address -g
LDFLAGS      += -fsanitize=address -g
endif

# ==============================================================================
# Top Level actions

release: ${BUILD}/libeve.a

test: compile_test ${BUILD}/test_main
	${BUILD}/test_main

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
.PRECIOUS: ${BUILD}/%_main.o

${BUILD}:
	mkdir -p ${BUILD}

${BUILD_OBJ}:
	mkdir -p ${BUILD_OBJ}

${BUILD}/test_main: ${BUILD}/test_main.o ${MODULES} ${TEST_MODULES} | ${BUILD}
	${CXX} -o $@ ${LDFLAGS} $^

${BUILD}/%_main: ${BUILD}/%_main.o ${BUILD}/libeve.a ${TEST_MODULES} | ${BUILD}
	${CXX} -o $@ ${LDFLAGS} $^

${BUILD}/%_main.o: cmd/%/main.cc | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ -c $^

${BUILD}/libeve.a: ${MODULES} | ${BUILD}
	${AR} $@ $^

${BUILD_OBJ}/testing_%.o: src/testing/%.cc | ${BUILD_OBJ}
	${CXX} ${CXXFLAGS} -o $@ -c $^

${BUILD_OBJ}/adt_%.o: src/adt/%.cc include/eve/adt/*.h | ${BUILD_OBJ}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${BUILD_OBJ}/base_%.o: src/base/%.cc include/eve/base/*.h | ${BUILD_OBJ}
	${CXX} ${CXXFLAGS} -o $@ -c $<
