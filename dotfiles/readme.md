## Install Configuration Files

### Install

Compile:

```
mkdir -p ~/Workspace
cd ~/Workspace
git clone git@github.com:xiejw/y.git
cd y/dotfiles
CC=clang make
```

### Bashrc

Use Bash for macOS

    chsh -s /bin/bash

Append the following block at the end of `~/.profile`.

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

### XMonad

_Likely_ deprecated.

- See [manpage](https://xmonad.org/manpage.html) for default key bindings.
- See [here](doc/xmonad_cheatsheet.md) for layout cheatsheet.

## Canonical Project Paths

LLVM
```
~/Workspace/build/llvm/src                # project
~/Workspace/build/llvm/build-<date>       # cmake build
~/Workspace/build/llvm/install            # install
```

TexLive
```
~/Workspace/build/texlive/<year>          # install (portable)
```

Libtorch
```
~/Workspace/build/torch/pyenv             # python venv
~/Workspace/build/torch/src               # project
~/Workspace/build/torch/build-<date>      # cmake build
~/Workspace/build/torch/install           # install
```
