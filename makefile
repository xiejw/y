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
	@echo "==>" install the following lines into ~/.profile
	@echo "##################################"
	@echo "# xiejw/y/dotfiles"
	@echo "export DOTFILES=~/Workspace/y/dotfiles"
	@echo "source ~/Workspace/y/dotfiles/conf/bash_profile"
	@echo "##################################"
