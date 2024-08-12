EVE_PATH            = ../../eve
EVE_FMT_FOLDERS     = .

include ${EVE_PATH}/eve.mk

SRCS                = tty.cc main.cc
LDFLAGS            += ${EVE_LIB}

.DEFAULT_GOAL       = compile

compile: ${BUILD}/main ${EVE_LIB}

${BUILD}/main: ${SRCS} | ${BUILD} ${EVE_LIB}
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

run: ${BUILD}/main compile
	${BUILD}/main

test: compile

release:
	${MK} RELEASE=1 compile
