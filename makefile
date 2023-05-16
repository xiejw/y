install:
	@echo "=>" install vimrc
	@echo "==>" delink sym links if any
	rm -f ~/.vim
	rm -f ~/.vimrc
	@echo "==>" link sym links
	ln -sf ~/Workspace/y/vimrc       ~/.vim
	ln -sf ~/Workspace/y/vimrc/vimrc ~/.vimrc
