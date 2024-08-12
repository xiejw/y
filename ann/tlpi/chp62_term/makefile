EVE_PATH            = ../../eve
EVE_FMT_FOLDERS     = .

include ${EVE_PATH}/eve.mk

SRCS                = tty.cc main.cc
LDFLAGS            += ${EVE_LIB}

.DEFAULT_GOAL       = compile

compile: ${BUILD}/main

${BUILD}/main: ${SRCS} | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

run: compile
	${BUILD}/main

test: compile

release:
	${MK} RELEASE=1 compile
