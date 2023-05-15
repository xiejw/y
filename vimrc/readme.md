# My vim configuration

## Cheat Sheet

See [here](cheatsheet.md).

## Instructions

### Install

- Clone and link the `vimrc`.

        cd ~/Workspace
        git clone git@github.com:xiejw/vimrc.git
        ln -sf ~/Workspace/vimrc ~/.vim
        ln -sf ~/.vim/vimrc ~/.vimrc

- Open vim and run `:PlugInstall`.
- Quit vim and open vim again.

### Configurations

```
# Env var
export FZF_DEFAULT_COMMAND=        # adjust the <leader>zz
export FZF_PENDING_FILE_COMMAND=   # adjust the <leader>ll
```

### Update

After sync to head, run
- `:PlugStatus` to check the plugs status
- `:PlugInstall` to install or `:PlugUpdate` to update if necessary
- `:PlugClean` to clean plugins.

