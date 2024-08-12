EVE_PATH            = ../../eve
EVE_FMT_FOLDERS     = .

include ${EVE_PATH}/eve.mk

.DEFAULT_GOAL       = compile

compile: ${BUILD}/main

${BUILD}/main: | ${BUILD}
	${CXX} ${CXXFLAGS} -o $@ tty.cc main.cc ${LDFLAGS}

run: compile
	${BUILD}/main

test: compile

release:
	${MK} RELEASE=1 compile
