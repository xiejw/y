BIN = .build/mdv

compile:
	mkdir -p .build
	go build -o $(BIN) ./cmd/...

release: compile

test:
	go test ./src/...

fmt:
	go fmt mdv/...

clean:
	rm -rf .build

install: compile
	mkdir -p ~/.local/bin
	cp ${BIN} ~/.local/bin

.PHONY: compile release test fmt clean install
