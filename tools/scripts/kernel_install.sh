#!/bin/bash
#
# This scripts install the compiled linux kernel. If
#
#    --modules_install   # Default false
#
# flag is passed, then the script will install modules as well before install
# kernel.
#
# NOTE
# - Please run this with normal user.
set -e
set -x

DIST=`grep ^NAME= /etc/os-release | awk -F = '{print $2}'`
echo "Distribution: $DIST"

arch_install() {
  KV=`make --no-print-directory kernelrelease`

  echo "Kernel version: ${KV}"

  echo "Copy all files to /boot {image, initramfs, config, system.map}."
  sudo cp -v arch/x86_64/boot/bzImage /boot/vmlinuz-linux-$KV
  sudo mkinitcpio -k $KV -g /boot/initramfs-linux-$KV.img || echo "The failure might be ok. Plz check warning/error message."
  sudo cp -v .config /boot/config-$KV
  sudo cp -v System.map /boot/System.map-${KV}

  echo "Update the grub."
  sudo grub-mkconfig -o /boot/grub/grub.cfg
  echo "Please check grub-mkconfig carefully!"
}

cd ~/Workspace/kernel/linux

if [[ "$1" == "--modules_install" ]]; then
  sudo make modules_install
fi

if [[ "$DIST" == *"Arch Linux"* ]]; then
  echo "Arch:  run script "
  arch_install

else
  echo "Other: run 'make install'"
  # For debian arm64
  # ```
  # find . -name 'Makefile' | xargs grep -nRH ^install: | less
  # less arch/arm64/Makefile
  # sudo make -n --debug=verbose install
  # less ./scripts/install.sh
  # ls /etc/kernel/postinst.d
  # ```

  sudo make install
fi
