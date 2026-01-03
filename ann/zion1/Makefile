.PHONY: compile fmt clean test
compile:
	make -C demo compile
	make -C c    compile

fmt:
	make -C demo fmt
	make -C cc   fmt
	make -C c    fmt

clean:
	go run ~/Workspace/y/tools/delete_unused_dirs.go

test: compile
