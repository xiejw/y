include eva.mk

# ==============================================================================
# configurations
# ==============================================================================
SRC             = src
CMD             = cmd
INCLUDE         = include

# public   headers are in INCLUDE
# internal headers are in SRC
CFLAGS         += -I${INCLUDE}
CFLAGS         += -I${SRC}

# MLVM needs MLVM_VM_INTERNAL guard
CFLAGS         += -DMLVM_VM_INTERNAL=1

# ------------------------------------------------------------------------------
# clang-format related, required by eva.mk
# ------------------------------------------------------------------------------
FMT_FOLDERS     = ${INCLUDE} ${SRC} ${CMD}

# ==============================================================================
# libs
# ==============================================================================
ADT_LIB         = ${BUILD}/adt_vec.o \
		  ${BUILD}/adt_sds.o \
                  ${BUILD}/adt_dict.o \
		  ${BUILD}/adt_value.o \
		  ${BUILD}/adt_list.o

ALGOS_LIB       = ${BUILD}/algos_regex.o

BASE_LIB        = ${BUILD}/base_error.o \
		  ${BUILD}/base_log.o \
		  ${BUILD}/base_bitops.o \
		  ${BUILD}/base_mm.o \
		  ${BUILD}/base_fpath.o

CRYPTO_LIB      = ${BUILD}/crypto_sha256.o \
		  ${BUILD}/crypto_rng64.o \
		  ${BUILD}/crypto_rng64_stdn.o

MLVM_LIB        = ${BUILD}/mlvm_kernel.o \
		  ${BUILD}/mlvm_shape.o \
		  ${BUILD}/mlvm_tensor.o \
		  ${BUILD}/mlvm_vm.o \


SCHED_LIB       = ${BUILD}/sched_cron.o \

ifdef PTHREAD
SCHED_LIB      += ${BUILD}/sched_fork_join.o
endif  # PTHREAD


ALL_LIBS        = ${ADT_LIB} \
		  ${ALGOS_LIB} \
		  ${BASE_LIB} \
		  ${CRYPTO_LIB} \
		  ${MLVM_LIB} \
		  ${SCHED_LIB} \


# ==============================================================================
# tests
# ==============================================================================
ADT_TEST        = ${BUILD}/adt_vec_test.o \
		  ${BUILD}/adt_sds_test.o \
		  ${BUILD}/adt_dict_test.o \
		  ${BUILD}/adt_list_test.o \
		  ${BUILD}/adt_value_test.o

ALGOS_TEST      = ${BUILD}/algos_regex_test.o

BASE_TEST       = ${BUILD}/base_error_test.o \
		  ${BUILD}/base_bitops_test.o \
		  ${BUILD}/base_mm_test.o \
		  ${BUILD}/base_fpath_test.o

CRYPTO_TEST     = ${BUILD}/crypto_sha256_test.o \
		  ${BUILD}/crypto_rng64_test.o \
		  ${BUILD}/crypto_rng64_stdn_test.o

MLVM_TEST       = ${BUILD}/mlvm_kernel_test.o \
		  ${BUILD}/mlvm_shape_test.o \
		  ${BUILD}/mlvm_tensor_test.o \
		  ${BUILD}/mlvm_vm_test.o \

SCHED_TEST      = ${BUILD}/sched_cron_test.o \


ifdef PTHREAD
SCHED_TEST     += ${BUILD}/sched_fork_join_test.o
endif # PTHREAD

ALL_TESTS       = ${ADT_TEST} \
		  ${ALGOS_TEST} \
		  ${BASE_TEST} \
		  ${CRYPTO_TEST} \
		  ${MLVM_TEST} \
		  ${SCHED_TEST}

# ==============================================================================
# actions
# ==============================================================================

.DEFAULT_GOAL = compile

all:
	@echo 'clean.'
	${MK} clean
	@echo 'compile non-release libs.'
	${MK} -B -j compile                          || exit 1
	@echo 'compile release libeva.'
	${MK} -B -j RELEASE=1 libeva                 || exit 1
	@echo 'compile release pthread libeva.'
	${MK} -B -j RELEASE=1 PTHREAD=1 libeva       || exit 1
	@echo 'run asan tests.'
	${MK} -B -j ASAN=1 test                      || exit 1
	@echo 'run asan pthread tests.'
	${MK} -B -j ASAN=1 PTHREAD=1 test            || exit 1

release:
	@echo 'clean.'
	${MK} clean
	@echo 'compile non-release libs.'
	${MK} -B -j compile                          || exit 1
	@echo 'compile release libeva.'
	${MK} -B -j RELEASE=1 libeva                 || exit 1
	@echo 'compile release pthread libeva.'
	${MK} -B -j RELEASE=1 PTHREAD=1 libeva       || exit 1

# incrementally compile all libraries
compile: ${BUILD} ${ALL_LIBS}

# libeva static library
libeva: compile ${BUILD}/libeva.a

${BUILD}/libeva.a: ${ALL_LIBS}
ifdef RELEASE
	${EVA_AR} $@ $^
else
	@echo "building libeva.a must be RELEASE mode."
endif  # RELEASE

# ==============================================================================
# rules for sub-modules
# ==============================================================================

${BUILD}/adt_%.o: ${SRC}/adt/%.c | ${BUILD}
	${EVA_CC} -o $@ -c $<

${BUILD}/algos_%.o: ${SRC}/algos/%.c | ${BUILD}
	${EVA_CC} -o $@ -c $<

${BUILD}/base_%.o: ${SRC}/base/%.c | ${BUILD}
	${EVA_CC} -o $@ -c $<

${BUILD}/crypto_%.o: ${SRC}/crypto/%.c | ${BUILD}
	${EVA_CC} -o $@ -c $<

MLVM_HEADERS       = ${INCLUDE}/mlvm/*.h ${SRC}/mlvm/*.h

${BUILD}/mlvm_%.o: ${SRC}/mlvm/%.c ${MLVM_HEADERS} | ${BUILD}
	${EVA_CC} -o $@ -c $<

${BUILD}/sched_%.o: ${SRC}/sched/%.c | ${BUILD}
	${EVA_CC} -o $@ -c $<

# ==============================================================================
# cmds and test
# ==============================================================================

# alias
c: cron
s: sudoku

compile: $(patsubst ${CMD}/%/main.c,${BUILD}/%,$(wildcard ${CMD}/*/main.c))

$(eval $(call objs,ansi_color,  $(BUILD),))
$(eval $(call objs,cron,  $(BUILD), ${BASE_LIB} ${ADT_LIB} ${SCHED_LIB}))
$(eval $(call objs,test,  $(BUILD), ${ALL_TESTS} ${ALL_LIBS}))
$(eval $(call objs,mlvm,  $(BUILD), ${BASE_LIB} ${ADT_LIB} ${CRYPTO_LIB} ${MLVM_LIB}))

# ==============================================================================
# docker
# ==============================================================================

DOCKER          = .docker
DOCKFILE        = cmd/cron/dockerfile.cron
DOCKER_BUILD_SH = cmd/cron/docker_build.sh

docker:
ifeq ($(UNAME), Linux)
	DOCKFILE=${DOCKFILE} BUILD_DIR=${BUILD_RELEASE} DOCKER_DIR=${DOCKER} \
		 ${DOCKER_BUILD_SH}
else
	@echo "building docker is supported on linux only."
endif  # Linux

clean_docker:
	@rm -rf ${DOCKER}

clean: clean_docker

