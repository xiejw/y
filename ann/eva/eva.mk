# ==============================================================================
# Global Knobs
#

# NO_CLANG_FLAGS disable customized clang flags
# NO_CLANG_ASAN  disable asan mode

# ------------------------------------------------------------------------------
# Required Inputs and Outputs
#
# Inputs
#   - EVA_PATH
#   - EVA_FMT_FOLDERS
#
# Outputs
#   - BUILD
#   - BUILD_OBJ
#   - EVA_LIB
#
# Actions:
#   - fmt
#   - clean
#   - ${EVA_LIB}

ifndef EVA_PATH
$(error EVA_PATH must be set)
endif  # EVA_PATH

ifndef EVA_FMT_FOLDERS
$(error EVA_FMT_FOLDERS must be set)
endif  # EVA_FMT_FOLDERS

# ==============================================================================
# Configurations
#

# ------------------------------------------------------------------------------
# Top level configurations
#
UNAME           = $(shell uname)
PROCESSOR       = $(shell uname -p)

BUILD_BASE      = .build
BUILD           = ${BUILD_BASE}
BUILD_OBJ       = ${BUILD}/objs

ifdef PHTREAD
EVA_LIB         = ${EVA_PATH}/.build_release_pthread/libeva.a
else
EVA_LIB         = ${EVA_PATH}/.build_release/libeva.a
endif

FMT             = ~/Workspace/y/tools/clang_format_all.sh

# ------------------------------------------------------------------------------
# Compiler flags
#

# --------------------------------
# Common flags used for all cases.
#
CFLAGS          += -std=c11 -Wall -Werror -pedantic -Wfatal-errors
CFLAGS          += -Wconversion -Wshadow
CFLAGS          += -I${EVA_PATH}/include

# --------------------------------
# all flags I am testing with clang.
# --------------------------------
#
# Use NO_CLANG_FLAGS=1 to skip.
ifndef NO_CLANG_FLAGS

CFLAGS          += -Wimplicit-int-conversion -Wimplicit-fallthrough
CFLAGS          += -Wimplicit-float-conversion -Wimplicit-int-float-conversion
CFLAGS          += -Wconversion -Waddress -Wcovered-switch-default
CFLAGS          += -Wextra-semi-stmt -Wcstring-format-directive
CFLAGS          += -Wno-unused-parameter -Wmissing-field-initializers
CFLAGS          += -Wmisleading-indentation -Wno-unused-parameter
CFLAGS          += -Wfloat-equal -Wcast-qual -Wshadow -Wsign-compare
CFLAGS          += -Wmissing-prototypes -Wswitch-enum -Wunused-macros
CFLAGS          += -Wmissing-variable-declarations -Wunreachable-code-return

# Flags I want to try, namely "-Weverything", which is super restricitve.
#
# Here, I am testing it now with explicit flags turned off by manual checking
# whether they make sense to my code.
CFLAGS          += -Weverything
CFLAGS          += -Wno-reserved-macro-identifier -Wno-reserved-identifier
CFLAGS          += -Wno-declaration-after-statement -Wno-cast-align
CFLAGS          += -Wno-format-nonliteral -Wno-padded
CFLAGS          += -Wno-bad-function-cast -Wno-strict-prototypes
CFLAGS          += -Wno-unsafe-buffer-usage
CFLAGS          += -Wno-covered-switch-default
CFLAGS          += -Wno-pre-c11-compat

else

# This is too restrict.
CFLAGS          += -Wno-strict-prototypes

endif  # NO_CLANG_FLAGS

# --------------------------------
# Error out asan if NO_CLANG_ASAN.
#
ifdef NO_CLANG_ASAN
ifdef ASAN
$(error asan mode is disabled by env NO_CLANG_ASAN)
endif  # ASAN
endif  # NO_CLANG_ASAN

# ------------------------------------------------------------------------------
# Linker flags
#
LDFLAGS         += -lm

# ------------------------------
# Enable POSIX and LLD for linux
# ------------------------------
#
# See
#
#   https://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
#
# In particular,
#
#   - '_POSIX_C_SOURCE' is used to enable POSIX functionality.
#   - '_DEFAULT_SOURCE' is used to enable certain BSD feature. Also see
#
#      https://sourceware.org/glibc/wiki/Release/2.20
#
ifeq ($(UNAME), Linux)
        CFLAGS  += -D_POSIX_C_SOURCE=201410L -D_DEFAULT_SOURCE
	LDFLAGS += -fuse-ld=lld
endif  # Linux

# ------------------------------------------------------------------------------
# Make
#
# Only BSD has different names. Linux is fine.
# ------------------------------------------------------------------------------
MK              = make

# ------------------------------------------------------------------------------
# Asan
#
# Enable by `make ASAN=1`
# ------------------------------------------------------------------------------

# Note memory leak is enabled by default on linux but needs ASAN_OPTIONS for
# macOS. See
#
#   https://clang.llvm.org/docs/AddressSanitizer.html
#
ifdef ASAN
	CFLAGS  += -fsanitize=address -D_ASAN=1
	BUILD   := ${BUILD}_asan

ifeq ($(UNAME), Darwin)
	EX      = ASAN_OPTIONS=detect_leaks=1

ifeq ($(PROCESSOR), arm)
	# macOS/M1 has some memory leaks while other platforms are ok. So, define
	# the special symbol _SKIP_ASAN the code can avoid them.
	CFLAGS  += -D_SKIP_ASAN=1
endif  # arm

endif  # Darwin

endif  # ASAN

# ------------------------------------------------------------------------------
# release
#
# Enable by `make RELEASE=1`
# ------------------------------------------------------------------------------
ifdef RELEASE
        CFLAGS  += -DNDEBUG -O2
        BUILD   := ${BUILD}_release

        # remove some checks as assert() does not work in RELEASE mode so assert
	# related macros will have trouble.
        CFLAGS  += -Wno-unused-macros

ifdef ASAN
$(error release mode does not support ASAN)
endif  # ASAN
else
        CFLAGS  += -g

endif  # RELEASE

# ------------------------------------------------------------------------------
# pthread
#
# Enable by `make PTHREAD=1`
# ------------------------------------------------------------------------------
ifdef PTHREAD
        CFLAGS  += -DPTHREAD
        LDFLAGS += -lpthread
        BUILD   := ${BUILD}_pthread
endif  # PTHREAD

# ==============================================================================
# Color printing
#
# Enable verbose by `make V=1`
#
EVA_CC          = ${QUIET_CC}${CC} ${CFLAGS}
EVA_LD          = ${QUIET_LD}${CC} ${LDFLAGS} ${CFLAGS}
EVA_AR          = ${QUIET_AR}ar -cr
EVA_EX          = ${QUIET_EX}${EX}
EVA_FM          = ${QUIET_FM}${FMT}

CCCOLOR         = "\033[34m"
LINKCOLOR       = "\033[34;1m"
SRCCOLOR        = "\033[33m"
BINCOLOR        = "\033[36;1m"
ENDCOLOR        = "\033[0m"

ifndef V
QUIET_CC  = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) \
				  $(SRCCOLOR)`basename $@`$(ENDCOLOR) 1>&2;
QUIET_LD  = @printf '    %b %b\n' $(LINKCOLOR)LD$(ENDCOLOR) \
				  $(BINCOLOR)`basename $@`$(ENDCOLOR) 1>&2;
QUIET_AR  = @printf '    %b %b\n' $(LINKCOLOR)AR$(ENDCOLOR) \
				  $(BINCOLOR)`basename $@`$(ENDCOLOR) 1>&2;
QUIET_EX  = @printf '    %b %b\n' $(LINKCOLOR)EX$(ENDCOLOR) \
				  $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_FM  = @printf '    %b %b\n' $(LINKCOLOR)FM$(ENDCOLOR) \
	                          $(BINCOLOR)"$(EVA_FMT_FOLDERS)"$(ENDCOLOR) 1>&2;
endif  # V

# ==============================================================================
# common actions
# ==============================================================================

fmt:
	${EVA_FM} ${EVA_FMT_FOLDERS}

clean:
	${EVA_EX} rm -rf ${BUILD_BASE}*

release: clean

${BUILD}:
	@mkdir -p ${BUILD}

${BUILD_OBJ}:
	mkdir -p ${BUILD_OBJ}

${EVA_LIB}:
	make -C ${EVA_PATH} release

# ==============================================================================
# Template to generate c binary rules
#

# The convention is for cmd <binary> the main file is cmd/<binary>/main.c.
#
# Arguments:
#   - $(2) is output dir,
#   - $(1) is binary name, and
#   - $(3) is library dep.
define objs

.PHONY: $(1)

$(1): compile $(2)/$(1)
	$$(EVA_EX) $(2)/$(1)

$(2)/$(1): cmd/$(1)/main.c $(3) | $(2)
	$$(EVA_LD) -o $$@ $$^

endef
