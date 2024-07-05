EVE_PATH            = ../../eve
EVE_FMT_FOLDERS     = .

include ${EVE_PATH}/eve.mk

.DEFAULT_GOAL       = compile

compile: ${BUILD}/main

${BUILD}/main: | ${BUILD}
	${CXX} ${CXXFLAGS} -o ${BUILD}/main tty.cc main.cc ${LDFLAGS}

run: compile
	${BUILD}/main

test: compile

release:
	make RELEASE=1 test
