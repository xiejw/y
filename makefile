# === WHAT ===
#
# makefile for project y
#
# === HOW ===
#
# if you are bootstrap a machine, run
#
#     make instal
#
# if you are sanity checking (daily run), run
#
#     make test
#
# if you are building eva, run
#
#     make eva      # alias
#     make libeva

.PHONY: install test eva libeva

test:
	@echo "run tests for all components" && \
		echo "==> ann/eva" && make -C ann/eva test > /tmp/y_eva.txt && \
		echo "==> ann/taocp/vol4" && make -C ann/taocp/vol4 test > /tmp/y_taocp_vol4.txt

eva: libeva

libeva:
	make -C ann/eva all

install:
	@echo "=>" install vimrc
	@echo "==>" delink sym links if any
	rm -f ~/.vim
	rm -f ~/.vimrc
	@echo "==>" link sym links
	ln -sf ~/Workspace/y/vimrc       ~/.vim
	ln -sf ~/Workspace/y/vimrc/vimrc ~/.vimrc
	@echo "==>" install plugins in vim
	@echo "##################################"
	@echo "$$ vim"
	@echo ":PlugInstall"
	@echo "##################################"
	@echo ""
	@echo "=>" install dotfiles
	@echo "==>" compile cmds
	CC=clang make -C dotfiles
	@echo "==>" link sym links for tmux
	rm -f ~/.tmux.conf
	ln -sf ~/Workspace/y/dotfiles/conf/tmux.conf ~/.tmux.conf
	@echo "==>" install the following lines into ~/.profile
	@echo "##################################"
	@echo "# xiejw/y/dotfiles"
	@echo "export DOTFILES=~/Workspace/y/dotfiles"
	@echo "source ~/Workspace/y/dotfiles/conf/bash_profile"
	@echo "##################################"

