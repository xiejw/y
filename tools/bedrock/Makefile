BUILD = /tmp/build

test:
	go build ./...

install:
	go run bedrock/cmd

fmt:
	go fmt ./...

# === --- For manually testing only ---------------------------------------- ===

test_all: test_toolchain test_c test_rs test_lib

test_toolchain:
	go run bedrock/cmd git                   ${BUILD}
	go run bedrock/cmd cmake                 ${BUILD}
	go run bedrock/cmd ninja                 ${BUILD}

# Tools written in c/cc
test_c:
	go run bedrock/cmd rsync                 ${BUILD}
	go run bedrock/cmd telnet                ${BUILD}
	go run bedrock/cmd bash                  ${BUILD}
	go run bedrock/cmd vim                   ${BUILD}
	go run bedrock/cmd tmux                  ${BUILD}

# Prebuilt binaries (Rust tools, downloaded; arm64-darwin and amd64-linux)
test_rs:
	go run bedrock/cmd fd                    ${BUILD}
	go run bedrock/cmd eza                   ${BUILD}
	go run bedrock/cmd ripgrep               ${BUILD}
	go run bedrock/cmd zoxide                ${BUILD}

test_lib:
	go run bedrock/cmd pcre2                 ${BUILD}

