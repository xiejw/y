EVE_PATH          = .
EVE_FMT_FOLDERS   = cmd include src
include eve.mk

MODULES          += ${BUILD_OBJ}/adt_sds.o
MODULES          += ${BUILD_OBJ}/base_error.o
MODULES          += ${BUILD_OBJ}/base_log.o

TEST_MODULES     += ${BUILD_OBJ}/adt_sds_test.o
TEST_MODULES     += ${BUILD_OBJ}/base_error_test.o
TEST_MODULES     += ${BUILD_OBJ}/base_result_test.o
TEST_MODULES     += ${BUILD_OBJ}/testing_testing_test.o
TEST_MODULES     += ${BUILD_OBJ}/testing_testing.o


# ==============================================================================
# Top Level actions
#

release:
	@echo 'compile release libeve'
	make -B -j RELEASE=1 libeve || exit 1

libeve: compile ${BUILD}/libeve.a

test: compile_test
	${BUILD}/test_main

# ==============================================================================
# Project actions
#

.DEFAULT_GOAL = compile

compile: compile_test

compile_test: ${BUILD}/test_main

# ==============================================================================
# Objects Actions
#
# see https://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: ${BUILD}/%_main.o

${BUILD}/test_main: ${BUILD}/test_main.o ${MODULES} ${TEST_MODULES} | ${BUILD}
	${CXX} -o $@ ${LDFLAGS} $^

${BUILD}/%_main.o: cmd/%/main.cc | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ -c $^

${BUILD}/libeve.a: ${MODULES} | ${BUILD}
ifdef RELEASE
	${AR} $@ $^
else
	@echo building libeve.a must be RELEASE mode.
	exit 1
endif

${BUILD_OBJ}/adt_%.o: src/adt/%.cc include/eve/adt/*.h | ${BUILD_OBJ}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${BUILD_OBJ}/base_%.o: src/base/%.cc include/eve/base/*.h | ${BUILD_OBJ}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${BUILD_OBJ}/testing_%.o: src/testing/%.cc | ${BUILD_OBJ}
	${CXX} ${CXXFLAGS} -o $@ -c $^

