## SOP

### Build Linux Kernel

Arch Linux:

- Download kernel from https://kernel.org/ and link as `kernel/linux`
- Copy config file as `kernel/linux/.config`
- Run

```
make menuconfig
grep =m .config | wc
time make [-j1]
sudo make modules_install
~/Workspace/y/dotfiles/tools/kernel_install.sh
```
