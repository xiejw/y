BUILD = /tmp/build

test: test_toolchain test_c test_rs test_go test_lib

test_toolchain:
	go run kit rust                  ${BUILD}
	go run kit git                   ${BUILD}
	go run kit cmake                 ${BUILD}
	go run kit ninja                 ${BUILD}

# Tools written in c/cc
test_c:
	go run kit rsync                 ${BUILD}
	go run kit telnet                ${BUILD}
	go run kit jq                    ${BUILD}
	go run kit bash                  ${BUILD}
	go run kit vim                   ${BUILD}

# Tools written in rust
test_rs:
	go run kit fd                    ${BUILD}
	go run kit eza                   ${BUILD}
	go run kit ripgrep               ${BUILD}
	go run kit zoxide                ${BUILD}

# Tools written in go
test_go:
	go run kit glow                  ${BUILD}

test_lib:
	go run kit pcre2                 ${BUILD}

install:
	go run kit
