BUILD    = .build

CFLAGS  += -std=c23 -O2 -DNDEBUG
CFLAGS  += -Wall -pedantic -Wextra -Wfatal-errors -Wconversion
CFLAGS  += -I.

compile: | ${BUILD}
	${CC} ${CFLAGS} -o ${BUILD}/tty.o -c tty.c

${BUILD}:
	mkdir -p ${BUILD}

clean:

test:

release:

fmt:
	~/Workspace/y/tools/clang_format_all.sh .


# EVE_PATH            = ../../eve
# EVE_FMT_FOLDERS     = .
#
# include ${EVE_PATH}/eve.mk
#
# LDFLAGS            += ${EVE_LIB}
#
# SRCS                = tty.cc main.cc
# BINS                = ${BUILD}/main
# DEPS                = ${BUILD} ${EVE_LIB}
#
# .PHONY: compile run test release
#
# .DEFAULT_GOAL       = compile
#
# compile: ${BINS} ${EVE_LIB}
#
# ${BUILD}/main: ${SRCS} | ${DEPS}
# 	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}
#
# run: ${BUILD}/main
# 	$<
#
# test: compile
#
# release:
# 	${MK} RELEASE=1 compile
