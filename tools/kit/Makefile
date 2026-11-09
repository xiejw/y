BUILD = /tmp/build

# cargo (rustup) is not on PATH by default; the Rust installers need it.
export PATH := $(HOME)/.cargo/bin:$(PATH)

test:
	go build ./...

install:
	go run kit/cmd

fmt:
	go fmt ./...

# === --- For manually testing only ---------------------------------------- ===

test_all: test_toolchain test_c test_rs test_lib

test_toolchain:
	go run kit/cmd git                   ${BUILD}
	go run kit/cmd cmake                 ${BUILD}
	go run kit/cmd ninja                 ${BUILD}

# Tools written in c/cc
test_c:
	go run kit/cmd rsync                 ${BUILD}
	go run kit/cmd telnet                ${BUILD}
	go run kit/cmd bash                  ${BUILD}
	go run kit/cmd vim                   ${BUILD}
	go run kit/cmd tmux                  ${BUILD}

# Tools written in rust
test_rs:
	go run kit/cmd fd                    ${BUILD}
	go run kit/cmd eza                   ${BUILD}
	go run kit/cmd ripgrep               ${BUILD}
	go run kit/cmd zoxide                ${BUILD}

test_lib:
	go run kit/cmd pcre2                 ${BUILD}

