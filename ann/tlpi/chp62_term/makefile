EVE_PATH            = ../../eve
EVE_FMT_FOLDERS     = .

include ${EVE_PATH}/eve.mk

LDFLAGS            += ${EVE_LIB}

SRCS                = tty.cc main.cc
BINS                = ${BUILD}/main
DEPS                = ${BUILD} ${EVE_LIB}

.PHONY: compile run test release

.DEFAULT_GOAL       = compile

compile: ${BINS} ${EVE_LIB}

${BUILD}/main: ${SRCS} | ${DEPS}
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

run: ${BUILD}/main
	$<

test: compile

release:
	${MK} RELEASE=1 compile
