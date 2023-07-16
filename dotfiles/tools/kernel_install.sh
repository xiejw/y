#!/bin/bash
#
DIST=`grep ^NAME= /etc/os-release | awk -F = '{print $2}'`
echo "Distribution: $DIST"

arch_install() {
  KV=`make kernelrelease`

  echo "Kernel version: ${KV}"

  echo "Copy all files to /boot {image, initramfs, config, system.map}."
  sudo cp -v arch/x86_64/boot/bzImage /boot/vmlinuz-linux-$KV
  sudo mkinitcpio -k $KV -g /boot/initramfs-linux-$KV.img
  sudo cp -v .config /boot/config-$KV
  sudo cp -v System.map /boot/System.map-${KV}

  echo "Update the grub."
  sudo grub-mkconfig -o /boot/grub/grub.cfg
  echo "Please check grub-mkconfig carefully!"
}

if [[ "$DIST" == *"Arch Linux"* ]]; then
  echo "Arch:  run script "
  arch_install()

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

  make install
fi
