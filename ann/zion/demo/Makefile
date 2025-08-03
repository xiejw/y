# === Configurations -----------------------------------------------------------
#
BUILD     = .build
SHELL     = bash
ZION_ROOT = ../cc
SRC       = ./

CXXFLAGS += -std=c++23 -Wall -Werror -pedantic -Wextra -Wfatal-errors -Wconversion
CXXFLAGS += -I${ZION_ROOT}/include

# === Rules --------------------------------------------------------------------
.PHONY: compile zion_demo

compile: zion_demo

zion_demo: ${BUILD}/zion_demo
	@echo "==> Now you can run `pwd`/$^ <=="

${BUILD}/zion_demo: ${SRC}/zion_demo.cc | ${BUILD}
	${CXX} -o $@ ${CXXFLAGS} $^ ${LDFLAGS}

# === House Keeping ------------------------------------------------------------
.PHONY: fmt clean

fmt:
	@~/Workspace/y/tools/clang_format_all.sh .

${BUILD}:
	@mkdir -p ${BUILD}

clean:
	rm -rf ${BUILD}
