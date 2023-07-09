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

### LLVM

#### Bootstrap

```
##########
# packages
##########
#
# debian
apt --no-install-recommends install xz-utils cmake ninja-build python3
# arch
pacman -S cmake ninja python3
```

#### Download

- https://releases.llvm.org/
- https://github.com/llvm/llvm-project.git


#### Compile

Target folder:
```
build/llvm/{src,install}
```

Build cmd:
```
mkdir build/llvm/build-<date>
cd build/llvm/build-<date>

sh ~/Workspace/y/dotfiles/tools/llvm_config.sh
time ninja [-j2]

rm build/llvm/install
ln -sf build/llvm/build-<date> build/llvm/install
```

### Go

#### Install

Go [official site](http://golang.org) to download. It is recommended **not** to
use Linux distro package to install, as it could be super old.

Typically, the install can be done via:

```
sudo tar -C /usr/local -xzf go$VERSION.$OS-$ARCH.tar.gz
```

### Libtorch

libtorch with mps on mac (amd64)

- ensure clang is built-in verison (not customized clang from source code)
- prepare the python env

```
python3 -m venv pyenv
./pyenv/bin/activate
pip3 install pyyaml typing_extensions
```

- follow the cmds:

```
cd build/torch
git clone --depth 1 --recursive https://github.com/pytorch/pytorch.git src-<date>
ln -sf src-<date> src
mkdir build-<date>
cd build-<date>
~/Workspace/y/dotfiles/tools/torch_config.sh
```

refs:
  https://pytorch.org/cppdocs/installing.html
  https://discuss.pytorch.org/t/compile-libtorch-c-api-from-source/81624
  https://github.com/pytorch/pytorch/blob/master/docs/libtorch.rst
  https://github.com/mlverse/libtorch-mac-m1
