.PHONY: compile test test
compile:
	make -C demo compile

fmt:
	make -C demo fmt
	make -C cc   fmt

clean:
	go run ~/Workspace/y/tools/delete_unused_dirs.go


test: compile
