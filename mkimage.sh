#!/bin/env bash

dd if=/dev/zero of=sol.img bs=512 count=32768
parted sol.img mklabel msdos # Add MBR
parted sol.img mkpart primary fat32 2048s 30720s # Create OS parition
parted sol.img set 1 boot on
LOOP0=$(sudo losetup -f)
sudo losetup ${LOOP0} sol.img
LOOP1=$(sudo losetup -f)
sudo losetup ${LOOP1} sol.img -o 1048576
sudo mkdosfs -F32 -f 2 ${LOOP1}
sudo mkdir /mnt/sol_grub
sudo mount ${LOOP1} /mnt/sol_grub
sudo grub2-install --target=i386-pc --boot-directory=/mnt/sol_grub --no-floppy --modules="normal part_msdos ext2 multiboot" ${LOOP0}
sudo umount /mnt/sol_grub
sudo rm -rf /mnt/sol_grub
sudo losetup -d ${LOOP0}
sudo losetup -d ${LOOP1}
