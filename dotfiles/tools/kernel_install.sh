#!/bin/sh

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
