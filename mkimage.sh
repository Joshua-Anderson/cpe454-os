#!/bin/env bash

dd if=/dev/zero of=$1 bs=512 count=32768
parted $1 mklabel msdos # Add MBR
parted $1 mkpart primary fat32 2048s 30720s # Create OS parition
parted $1 set 1 boot on
LOOP0=$(sudo losetup -f)
sudo losetup ${LOOP0} $1
LOOP1=$(sudo losetup -f)
sudo losetup ${LOOP1} $1 -o 1048576
sudo mkdosfs -F32 -f 2 ${LOOP1}
sudo mkdir /mnt/sol_grub
sudo mount ${LOOP1} /mnt/sol_grub
sudo grub2-install --target=i386-pc --boot-directory=/mnt/sol_grub/boot --no-floppy --modules="normal part_msdos ext2 multiboot" ${LOOP0}
sudo cp -r $2/* /mnt/sol_grub
sudo umount /mnt/sol_grub
sudo rm -rf /mnt/sol_grub
sudo losetup -d ${LOOP0}
sudo losetup -d ${LOOP1}
