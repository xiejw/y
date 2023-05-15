# Cheat Sheet

## General

| Category   | Key               | Comment                                              |
| ---------- | ------------------| -----------------------------------------------------|
| Switches   | `<leader>-r`      | Refresh the file content                             |
|            | `<leader>-s`      | Open spell check                                     |
|            | `<leader>-ns`     | Close spell check                                    |
|            | `<leader>-p`      | Enable `paste` (disabled now)                        |
|            | `<leader>-np`     | Disable `paste`  (disabled now)                      |
| Navigation | `<leader>-b`      | Switch to next buffer                                |
|            | `<leader>-B`      | Switch to previous buffer                            |
|            | `<leader>-t`      | Switch to next tab                                   |
|            | `<leader>-T`      | Switch to previous tab                               |
| Listing    | `<leader>-ll`     | List local pending files (git)                       |
|            | `<leader>-lb`     | List recent files.                                   |
|            | `<leader>-zz`     | List files via FZF.                                  |
| Open       | `<leader>-ee`     | Try to open a file in local folder in current tab.   |
|            | `<leader>-et`     | Try to open a file in local folder in new tab.       |
| Special    | `` C-[ ``         | ESC in insert mode (vim default).                    |
|            | `` <leader>-` ``  | Add `` ` `` to the start and end of the current word.|
|            | `` <leader>-d ``  | Kill buffer without losing split.                    |

## Window Cheat Sheet

| Category   | Keys              | Comment                                              |
| ---------- | ------------------| -----------------------------------------------------|
| Resize w   | `C-w >`           | increase width                                       |
| Resize w   | `C-w <`           | reduce   width                                       |
| Resize h   | `C-w +`           | increase height                                      |
| Resize h   | `C-w -`           | reduce   height                                      |

## Folding Cheat Sheet

This is the Vim built-in folding commands. Just copy here for quick reference.

| Category   | Keys              | Comment                                              |
| ---------- | ------------------| -----------------------------------------------------|
| Foldlevel  | `zi`              | switch folding on or off                             |
|            | `zm`              | increases the foldlevel by one.                      |
|            | `zM`              | closes all open folds.                               |
|            | `zr`              | decreases the foldlevel by one.                      |
|            | `zR`              | decreases the foldlevel to zero -- open everything.  |
| Open/Close | `za`              | toggle current fold open/closed                      |
|            | `zA`              | recursively open/close current fold                  |
|            | `zv`              | expand folds to reveal cursor                        |
|            | `zo`              | open current fold                                    |
|            | `zO`              | recursively open current fold                        |
|            | `zc`              | close current fold                                   |
|            | `zC`              | recursively close current fold                       |
| Move/Jump  | `zj`              | moves the cursor to the next fold.                   |
|            | `zk`              | moves the cursor to the previous fold.               |
|            | `[z`              | move to start of open fold.                          |
|            | `]z`              | move to end of open fold.                            |
| Reset      | `zX`              | undo manually opened and closed folds.               |
|            | `zx`              | `zX` then `zv`.                                      |
| Mapping    | `<space>`         | `zO`.                                                |
|            | `<space><space>`  | `zX`.                                                |


## Unusual Shortcuts

    "*y     Copy the content to system clipboard.

