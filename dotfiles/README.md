## Install Configuration Files

### Install

Shortcut
```
# At y root dir
make install
```

Or compile manually (requires Go)
```
mkdir -p ~/Workspace
cd ~/Workspace
git clone git@github.com:xiejw/y.git
cd y/dotfiles
make install
```

This builds the Go cmds under `cmds/` (`prompt`, `sync_git_repo`) and installs
them into `~/.local/bin`. Use `make install BIN_DIR=<dir>` to install elsewhere.

### Bashrc

Use Bash for macOS

    sudo vim /etc/shells
    chsh

Append the following block at the end of `~/.profile`.

    # Automatically
    go run ~/Workspace/y/tools/scripts/patch_profile.go

    # Manually
    # Add this block

    # xiejw/y/dotfiles
    export DOTFILES=~/Workspace/y/dotfiles
    source ~/Workspace/y/dotfiles/conf/bash_profile

### Tmux

Link the file:

    ln -sf ~/Workspace/y/dotfiles/conf/tmux.conf ~/.tmux.conf

- See [here](../doc/tmux.md#tmux-cheatsheet) for cheatsheet.
- See [here](../doc/tmux.md#install-from-source-code) for installing from source code.

### Kitty

    # install font SF Mono from https://developer.apple.com/fonts/

    mkdir -p  ~/.config/kitty
    rm ~/.config/kitty/kitty.conf
    ln -sf ~/Workspace/y/dotfiles/conf/kitty.conf ~/.config/kitty/kitty.conf

