## SOP

### Linux Kernel

#### Bootstrap Linux Kernel Config

```
##########
# packages
##########
#
# debian
sudo apt-get install --no-install-recommends build-essential bc kmod flex libncurses5-dev libelf-dev libssl-dev dwarves bison
# arch
sudo pacman -S base-devel python bc pahole

##########
# bootstrap config
##########
#
lsmod > /tmp/lsmod.txt
make LSMOD=/tmp/mylsmod localmodconfig
make menuconfig
  - CONFIG_LOCALVERSION="-xiejw"

# now go to next subsection.
```

#### Build Linux Kernel

Arch Linux:

- Download kernel from https://kernel.org/ and link as `kernel/linux`
- Copy config file as `kernel/linux/.config`
- Run

```
make menuconfig
grep =m .config | wc
time make [-j1]
sudo make modules_install

# debian
make install
# arch
~/Workspace/y/dotfiles/tools/kernel_install.sh
```

### Go

#### Install

Go [official site](http://golang.org) to download. It is recommended **not** to
use Linux distro package to install, as it could be super old.

Typically, the install can be done via:

```
sudo tar -C /usr/local -xzf go$VERSION.$OS-$ARCH.tar.gz
```
