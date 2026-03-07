## Install Configuration Files

### Install

Shortcut
```
# At y root dir
make install
```

Or compile manually
```
mkdir -p ~/Workspace
cd ~/Workspace
git clone git@github.com:xiejw/y.git
cd y/dotfiles
make
```

### Bashrc

Use Bash for macOS

    sudo vim /etc/shells
    chsh

Append the following block at the end of `~/.profile`.

    # Automatically
    go run tools/scripts/patch_profile.go

    # Manually
    # Add this block

    # xiejw/y/dotfiles
    export DOTFILES=~/Workspace/y/dotfiles
    source ~/Workspace/y/dotfiles/conf/bash_profile

### Tmux

Link the file:

    ln -sf ~/Workspace/y/dotfiles/conf/tmux.conf ~/.tmux.conf

- See [here](doc/tmux.md#tmux-cheatsheet) for cheatsheet.
- See [here](doc/tmux.md#install) for installing from source code.

### Kitty

    # install font SF Mono from https://developer.apple.com/fonts/

    mkdir -p  ~/.config/kitty
    rm ~/.config/kitty/kitty.conf
    ln -sf ~/Workspace/y/dotfiles/conf/kitty.conf ~/.config/kitty/kitty.conf
    ln -sf ~/Workspace/y/dotfiles/conf/zoom_toggle.py ~/.config/kitty/

