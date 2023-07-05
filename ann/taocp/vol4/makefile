EVA_PATH        = ../../eva

ifdef PHTREAD
EVA_LIB         = ${EVA_PATH}/.build_release_pthread/libeva.a
else
EVA_LIB         = ${EVA_PATH}/.build_release/libeva.a
endif

include ${EVA_PATH}/eva.mk

# ------------------------------------------------------------------------------
# configurations
# ------------------------------------------------------------------------------

CMD             =  cmd
SRC             =  src
INC             =  include

CFLAGS          += -I${EVA_PATH}/include -I${SRC} -I${INC}
LDFLAGS         += ${EVA_LIB}

BUILD_OBJS      = ${BUILD}/objs

# ------------------------------------------------------------------------------
# configurations, required by eva.mk
# ------------------------------------------------------------------------------
FMT_FOLDERS     =  ${CMD} ${SRC} ${INC}

# ------------------------------------------------------------------------------
# libs
# ------------------------------------------------------------------------------
ALGOS_LIBS      = ${BUILD_OBJS}/algos_dal.o \
		  ${BUILD_OBJS}/algos_horn.o

ALL_LIBS       += ${ALGOS_LIBS}

# ------------------------------------------------------------------------------
# actions
# ------------------------------------------------------------------------------

.DEFAULT_GOAL   = compile

${BUILD_OBJS}: | ${BUILD}
	@mkdir -p ${BUILD_OBJS}

# incrementally compile all libraries.
compile: ${ALL_LIBS}

all:
	@echo 'clean.'
	${MK} clean
	@echo 'compile non-release libs.'
	${MK} -B -j compile                          || exit 1
	@echo 'compile release libs.'
	${MK} -B -j RELEASE=1 || exit 1
	@echo 'compile release pthread libs.'
	${MK} -B -j RELEASE=1 PTHREAD=1 || exit 1
	@echo 'run asan tests.'
	${MK} -B -j ASAN=1 test                      || exit 1
	@echo 'run asan pthread tests.'
	${MK} -B -j ASAN=1 PTHREAD=1 test            || exit 1

# ------------------------------------------------------------------------------
# rules for sub-modules
# ------------------------------------------------------------------------------
${BUILD_OBJS}/algos_%.o: ${SRC}/algos/%.c ${INC}/algos/*.h | ${BUILD_OBJS}
	${EVA_CC} -o $@ -c $<

# ------------------------------------------------------------------------------
# cmds
# ------------------------------------------------------------------------------

compile: $(patsubst ${CMD}/%/main.c,${BUILD}/%,$(wildcard ${CMD}/*/main.c))

$(eval $(call objs,horn,  $(BUILD), ${ALGOS_LIBS}))
$(eval $(call objs,sudoku,$(BUILD), ${ALGOS_LIBS}))

# ------------------------------------------------------------------------------
# tests
# ------------------------------------------------------------------------------

# unit tests
TEST_LIBS       = ${BUILD_OBJS}/algos_dal_test.o \
		  ${BUILD_OBJS}/algos_horn_test.o \

$(eval $(call objs,test,$(BUILD),$(ALL_LIBS) $(TEST_LIBS)))

