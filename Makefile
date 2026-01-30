# === WHAT ===
#
# Makefile for project y
#
# === BOOTSTRAP ===
#
#     make install
#
# === HOW ===
#
# If you are sanity checking (daily run), run
#
#     make fmt
#     make test
#     make clean


WE_ARE_GOOD = printf "\n\e[1;32m>>> We are good!!! <<<\e[0m\n"

.PHONY: fmt test clean install

# ------------------------------------------------------------------------------
# Maintaince actions

fmt:
	make -C dotfiles                  fmt         && \
	make -C tools                     fmt         && \
	${WE_ARE_GOOD}

test:
	${WE_ARE_GOOD}

clean:
	go run tools/delete_unused_dirs.go

# ------------------------------------------------------------------------------
# Install
#

install:
	@echo "=>" Install vimrc
	@echo "==>" Delink sym links if any
	rm -f ~/.vim
	rm -f ~/.vimrc
	@echo "==>" Link sym links
	ln -sf ~/Workspace/y/vimrc       ~/.vim
	ln -sf ~/Workspace/y/vimrc/vimrc ~/.vimrc
	@echo "==>" Install plugins in vim
	vim -c ":PlugInstall" -c ":sleep 3" -c ":qa"
	@echo ""
	@echo "=>" Install dotfiles
	@echo "==>" Compile cmds
	make -C dotfiles
	@echo "==>" Link sym links for tmux
	rm -f ~/.tmux.conf
	ln -sf ~/Workspace/y/dotfiles/conf/tmux.conf ~/.tmux.conf
	@echo "==>" Patch ~/.profile
	python3 ~/Workspace/y/tools/patch_profile_for_y.py

