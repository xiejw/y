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

CC    ?= clang
MK     = make --no-print-directory

ifdef V
STDOUT_ROUTE = | tee
else
STDOUT_ROUTE = >
endif

.PHONY: install test eva libeva_release libeva_all

test:
	@echo "${C_ALRT}!!!! run tests for all components (/tmp/y_<project_name>.txt)${C_REST}" && \
		$(call TEST_DIR,ann/eva)  && \
		$(call TEST_DIR,ann/eve)  && \
		$(call TEST_DIR,ann/luna) && \
		$(call TEST_DIR,ann/taocp/vol4) && \
		echo "${C_ALRT}DONE${C_REST}"

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


fmt:
	@$(call FMT_DIR, dotfiles)
	@$(call FMT_DIR, tools)
	@$(call FMT_DIR, ann/eve)
	@$(call FMT_DIR, ann/eva)

clean:
	find . -name '.build*' -type d  | xargs rm -rf && \
		find . -not -wholename './.git/*' -not -wholename './vimrc/plugged/*' -type d -empty -delete

# ==============================================================================
# templates
# ==============================================================================

# Define a FMT_DIR func
# Usage:
#     $(call FMT_DIR,dir_name)
define FMT_DIR
	@echo "${C_INFO}fmt $(1)${C_REST}"
	${MK} -C $(1) fmt
endef

# Define a TEST_DIR func
# Usage:
#     $(call TEST_DIR,dir_name)
define TEST_DIR
        echo "${C_INFO}<==> `printf "%-10s" $(1)` ${C_REST}" && make -C $(1) test ${STDOUT_ROUTE} /tmp/y_`basename $(1)`.txt
endef
