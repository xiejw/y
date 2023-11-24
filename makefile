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
#     make eva             # alias
#     make libeva_release
#     make libeva_all

C_ALRT = \033[0;33m
C_INFO = \033[0;32m
C_REST = \033[0m

.PHONY: install test eva libeva_release libeva_all

test:
	@echo "${C_ALRT}!!!! run tests for all components (/tmp/y_<project_name>.txt)${C_REST}" && \
		echo "${C_INFO}<==> ann/eva        ${C_REST}" && make -C ann/eva        test > /tmp/y_eva.txt         && \
		echo "${C_INFO}<==> ann/taocp/vol4 ${C_REST}" && make -C ann/taocp/vol4 test > /tmp/y_taocp_vol4.txt  && \
		echo "${C_INFO}<==> ann/luna       ${C_REST}" && make -C ann/luna       test > /tmp/y_luna.txt        && \
		echo "${C_INFO}<==> bye            ${C_REST}"

eva: libeva_release

libeva_release:
	make -C ann/eva release

libeva_all:
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

clean:
	find . -name '.build*' -type d  | xargs rm -rf && \
		find . -not -wholename './.git/*' -not -wholename './vimrc/plugged/*' -type d -empty -delete
