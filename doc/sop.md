# SOP

## Linux Kernel

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
sudo ~/Workspace/y/dotfiles/tools/kernel_install.sh
```

## LLVM

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

## Go

<details>
  <summary>Click me</summary>

  #### Install

  Go [official site](http://golang.org) to download. It is recommended **not** to
  use Linux distro package to install, as it could be super old.

  Typically, the install can be done via:

  ```
  sudo tar -C /usr/local -xzf go$VERSION.$OS-$ARCH.tar.gz
  ```
</details>

## Libtorch

libtorch with mps on mac (amd64) and debian (amd64).

- ensure `clang` is built-in verison (not customized clang from source code)
- prepare the python venv

```
# (pip) prepare the venv
cd ~/Workspace/build
python3 -m venv pyenv
source ./pyenv/bin/activate
# (conda)
conda create -n <env_name>
conda activate  <env_name>

# required for compiling libtorch
pip3 install pyyaml typing_extensions
conda install pyyaml typing_extensions

# optional for testing pytorch
pip3 install -U torch numpy
# if [cpu] only
pip3 install torch --index-url https://download.pytorch.org/whl/cpu
```

- follow the cmds:

```
#
# prepare source code
#
cd build/torch
git clone --recursive https://github.com/pytorch/pytorch.git git
ln -sf git src

#
# prepare cmake with ninja
#
mkdir build-<date>
cd build-<date>

# macOS
~/Workspace/y/tools/torch_config.sh

# debian
~/Workspace/y/tools/torch_config_debian.sh

#
# compile and install with ninja
#
time ninja
ninja install

#
# finish the install
#
cd ..
ln -sf install-<data> install
```

Refs:
- https://pytorch.org/cppdocs/installing.html
- https://discuss.pytorch.org/t/compile-libtorch-c-api-from-source/81624
- https://github.com/pytorch/pytorch/blob/master/docs/libtorch.rst
- https://github.com/mlverse/libtorch-mac-m1
