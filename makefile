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
#     make clean
#     make test


WE_ARE_GOOD = printf "\n\e[1;32m>>> We are good!!! <<<\e[0m\n"

.PHONY: fmt test clean install

# ------------------------------------------------------------------------------
# Maintaince actions

fmt:
	make -C dotfiles                  fmt         && \
	make -C tools                     fmt         && \
	make -C ann/eva                   fmt         && \
	make -C ann/eve                   fmt         && \
	make -C ann/luna                  fmt         && \
	make -C ann/taocp/v4-dancing-link fmt         && \
	make -C ann/taocp/v4-horn-clause  fmt         && \
	make -C ann/tlpi/chp62_term       fmt         && \
	${WE_ARE_GOOD}

test:
	make -C ann/eva                   test        && \
	make -C ann/eve                   test        && \
	make -C ann/luna                  test        && \
	make -C ann/taocp/v4-dancing-link test        && \
	make -C ann/taocp/v4-horn-clause  test        && \
	make -C ann/tlpi/chp62_term       test        && \
	${WE_ARE_GOOD}

clean:
	go run tools/delete_unused_dirs.go

# ------------------------------------------------------------------------------
# Install
#

install:
	@echo "=>" install vimrc
	@echo "==>" delink sym links if any
	rm -f ~/.vim
	rm -f ~/.vimrc
	@echo "==>" link sym links
	ln -sf ~/Workspace/y/vimrc       ~/.vim
	ln -sf ~/Workspace/y/vimrc/vimrc ~/.vimrc
	@echo "==>" install plugins in vim
	vim -c ":PlugInstall" -c ":sleep 3" -c ":qa"
	@echo ""
	@echo "=>" install dotfiles
	@echo "==>" compile cmds
	make -C dotfiles
	@echo "==>" link sym links for tmux
	rm -f ~/.tmux.conf
	ln -sf ~/Workspace/y/dotfiles/conf/tmux.conf ~/.tmux.conf
	@echo "==>" install the following lines into ~/.profile
	@echo "##################################"
	@echo "# xiejw/y/dotfiles"
	@echo "export DOTFILES=~/Workspace/y/dotfiles"
	@echo "source ~/Workspace/y/dotfiles/conf/bash_profile"
	@echo "##################################"


