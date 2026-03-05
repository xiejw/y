BUILD = /tmp/build

test: test_toolchain test_c test_rs test_go test_lib

test_toolchain:
	go run install-tools rust                  ${BUILD}
	go run install-tools git                   ${BUILD}
	go run install-tools cmake                 ${BUILD}
	go run install-tools ninja                 ${BUILD}

# Tools written in c/cc
test_c:
	go run install-tools rsync                 ${BUILD}
	go run install-tools telnet                ${BUILD}
	go run install-tools jq                    ${BUILD}
	go run install-tools bash                  ${BUILD}
	go run install-tools vim                   ${BUILD}

# Tools written in rust
test_rs:
	go run install-tools fd                    ${BUILD}
	go run install-tools eza                   ${BUILD}
	go run install-tools ripgrep               ${BUILD}
	go run install-tools zoxide                ${BUILD}

# Tools written in go
test_go:
	go run install-tools glow                  ${BUILD}

test_lib:
	go run install-tools pcre2                 ${BUILD}

install:
	go run install-tools
