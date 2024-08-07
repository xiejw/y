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

CXXFLAGS         += -std=c++20 -Wall -pedantic -Wextra -Wfatal-errors
CXXFLAGS         += -Wconversion -Wshadow
CXXFLAGS         += -Iinclude

FMT               = ~/Workspace/y/tools/clang_format_all.sh
AR                = ar -cr

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

ifdef ASAN
# Ref: https://clang.llvm.org/docs/AddressSanitizer.html
CXXFLAGS         += -fsanitize=address -g
LDFLAGS          += -fsanitize=address -g
BUILD             = ${BUILD_BASE}_asan
endif  # ASAN

# ------------------------------------------------------------------------------
# Common Actions
#
fmt:
	${FMT} ${EVE_FMT_FOLDERS}

clean:
	rm -rf ${BUILD} ${BUILD_OBJ}

release: clean

${BUILD}:
	mkdir -p ${BUILD}

${BUILD_OBJ}:
	mkdir -p ${BUILD_OBJ}

${EVE_LIB}:
	make -C ${EVE_PATH} release
