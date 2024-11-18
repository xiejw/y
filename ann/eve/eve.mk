# ------------------------------------------------------------------------------
# Required Inputs and Outputs
#
# Inputs
#   - EVE_PATH
#   - EVE_FMT_FOLDERS
#
# Outputs
#   - BUILD
#   - BUILD_OBJ
#   - EVE_LIB
#
# Actions:
#   - fmt
#   - clean
#   - ${EVE_LIB}

ifndef EVE_PATH
$(error EVE_PATH must be set)
endif  # EVE_PATH

ifndef EVE_FMT_FOLDERS
$(error EVE_FMT_FOLDERS must be set)
endif  # EVE_FMT_FOLDERS

# ------------------------------------------------------------------------------
# Configuraitons
#
BUILD_BASE        = .build
BUILD             = ${BUILD_BASE}
BUILD_OBJ         = ${BUILD}/objs

CXXFLAGS         += -std=c++23 -Wall -pedantic -Wextra -Wfatal-errors
CXXFLAGS         += -Wconversion -Wshadow
CXXFLAGS         += -I${EVE_PATH}/include

FMT               = ~/Workspace/y/tools/clang_format_all.sh
AR                = ar -cr
MK                = make --no-print-directory

EVE_LIB           = ${EVE_PATH}/.build_release/libeve.a

# ------------------------------------------------------------------------------
# Knobs
#
ifndef SKIP_WERROR
CXXFLAGS         += -Werror
endif  # SKIP_WERROR

ifdef RELEASE
CXXFLAGS         += -DNDEBUG -O2
BUILD             = ${BUILD_BASE}_release

ifdef ASAN
$(error release mode does not support ASAN)
endif  # ASAN
endif  # RELEASE

ifndef RELEASE
CXXFLAGS         += -g
endif  # RELEASE

ifdef ASAN
# Ref: https://clang.llvm.org/docs/AddressSanitizer.html
CXXFLAGS         += -fsanitize=address
LDFLAGS          += -fsanitize=address
BUILD             = ${BUILD_BASE}_asan
endif  # ASAN

# ------------------------------------------------------------------------------
# Common Actions
#
fmt:
	${FMT} ${EVE_FMT_FOLDERS}

clean:
	rm -rf ${BUILD_BASE}*

release: clean

${BUILD}:
	mkdir -p ${BUILD}

${BUILD_OBJ}:
	mkdir -p ${BUILD_OBJ}

# For most projects, EVE_LIB should be set so they can use it as a dependency.
# For the EVE itself (EVE_ROOT defined), this is not overriden
ifndef EVE_ROOT
${EVE_LIB}:
	make -C ${EVE_PATH} release
endif
