# vim:

CMDS = prompt sync_git_repo

BIN_DIR ?= $(HOME)/.local/bin

.PHONY: default install $(CMDS) fmt test clean

# -------
# actions
#
default: install

install: $(CMDS)

# Each cmd lives at <name>/cmd/main.go and installs to $(BIN_DIR)/<name>.
$(CMDS):
	mkdir -p $(BIN_DIR) && go build -o $(BIN_DIR)/$@ ./$@/cmd/main.go

# -----------
# maintenance
#
fmt:
	gofmt -w $(CMDS)

test: default

clean:
	rm -f $(addprefix $(BIN_DIR)/,$(CMDS))
