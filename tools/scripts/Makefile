fmt:
	gofmt -w -l *.go

# Some binaries are only targeted macOS.
test:
	GOOS=darwin GOARCH=arm64 fd -e go -x go build -o /dev/null
