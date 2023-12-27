# SOP

## Linux Kernel

<details>
  <summary>Click me</summary>

  #### Bootstrap Linux Kernel Config

  ```
  #
  ### packages
  #
  # debian
  sudo apt-get install --no-install-recommends build-essential bc kmod flex libncurses5-dev libelf-dev libssl-dev dwarves bison
  # arch
  sudo pacman -S base-devel python bc pahole

  #
  ### bootstrap config
  #
  lsmod > /tmp/lsmod.txt
  make LSMOD=/tmp/lsmod.txt localmodconfig
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

</details>

## Linux Kernel for Docker Support

<details>
  <summary>Click me</summary>

  Refs:
  - https://wiki.archlinux.org/title/docker
  - https://wiki.gentoo.org/wiki/Docker
  - https://ungleich.ch/en-us/cms/blog/2018/08/18/iptables-vs-nftables/

  Minimal changes for Docker support
  ```
    [*] Networking support  --->
          Networking options  --->
            <*> 802.1d Ethernet Bridging
            [*] Network packet filtering framework (Netfilter)  --->
                [*] Advanced netfilter configuration
                [*]  Bridged IP/ARP packets filtering
                    IP: Netfilter Configuration  --->
                      <*> IP tables support (required for filtering/masq/NAT)
                      <*>   Packet filtering
                      <*>   iptables NAT support
                      <*>     MASQUERADE target support
                    Core Netfilter Configuration  --->
                      <*> Netfilter connection tracking support
                      <*> Connection tracking netlink interface
                      *** Xtables matches ***
                      <*>   "addrtype" address type match support
                      <*>   "conntrack" connection tracking match support
    Device Drivers  --->
        [*] Multiple devices driver support (RAID and LVM)  --->
            <*>   Device mapper support
            <*>     Thin provisioning target
        [*] Network device support  --->
            [*]   Network core driver support
            <*>     Virtual ethernet pair device
    File systems  --->
        <*> Overlay filesystem support
        Pseudo filesystems  --->
            [*] HugeTLB file system support
  ```


  If `nf_tables` is used instead of legacy `iptables` (by run `sudo iptables`),
  then enable the following options

  ```
    [*] Networking support  --->
          Networking options  --->
            [*] Network packet filtering framework (Netfilter)  --->
                    Core Netfilter Configuration  --->
                        <*> Netfilter nf_tables support
                            <*> Netfilter nf_tables conntrack module
                            <*> Netfilter nf_tables masquerade support
                            <*> Netfilter nf_tables nat support
                            <*> Netfilter x_tables over nf_tables module
  ```

  FAQ:
  1. Module sign
     ```
     # error
     make[1]: *** No make rule for making targets <n>, needed for <certs/x509_certificate_list>.  Stop.

     # fix
     CONFIG_MODULE_SIG_KEY="certs/signing_key.pem"
     CONFIG_SYSTEM_TRUSTED_KEYS=""
     ```



</details>

## LLVM

<details>
  <summary>Click me</summary>

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
</details>

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

## Postgresql (psql)

<details>
  <summary>Click me</summary>

  Refs:
  - https://www.postgresql.org/ftp/source/v16.1/
  - https://www.postgresql.org/docs/current/install-make.html

  #### Bootstrap

  ```
  #
  ### packages
  #
  # debian
  apt --no-install-recommends install libicu-dev
  # arch
  #
  # macOS
  brew install icu4c
  ```

  #### Compile and Install

  ```
  # compile
  cd build/psql/src
  ## next line is needed for macOS brew
  export PKG_CONFIG_PATH="/opt/homebrew/opt/icu4c/lib/pkgconfig"
  ./configure --prefix=`pwd`/../build-2023-<date>
  make

  # install
  make install
  cd ..
  ln -sf build-<date> install
  ```
  #### Test

  ```
  mkdir data16
  ./install/bin/initdb -D ./data16/
  cat data16/postgresql.conf  # change port 5433
  install/bin/pg_ctl -D ./data16/ -l logfile start
  tail -f logfile
  ./install/bin/psql -p 5433 -d postgres
  select current_time;
  ```

</details>

## LibTorch

<details>
  <summary>Click me</summary>

  libtorch with mps on mac (amd64) and debian (amd64).

  - ensure `clang` is built-in verison (not customized clang from source code)
  - prepare the python venv

  ```
  #
  # prepare the python venv
  #
  cd ~/Workspace/build
  python3 -m venv pyenv
  source ./pyenv/bin/activate

  # required for compiling libtorch
  pip3 install pyyaml typing_extensions

  #
  # optional for testing pytorch
  #
  pip3 install -U torch numpy
  pip3 install torch --index-url https://download.pytorch.org/whl/cpu   # if [cpu] only
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

  ~/Workspace/y/tools/torch_config.sh              # macOS
  ~/Workspace/y/tools/torch_config_debian.sh       # debian

  #
  # compile and install with ninja
  #
  time ninja -j4
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
</details>
