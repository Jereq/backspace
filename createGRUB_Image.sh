#!/bin/sh
DISK_SIZE=32M
BUILD_DIR=build
IMAGE=$BUILD_DIR/disk.img
dd of=$IMAGE bs=1 count=0 seek=$DISK_SIZE
parted --script $IMAGE mklabel msdos mkpart primary 1MB 100%
DEV_LOOP=`losetup --find --show $IMAGE`
echo "Image loopback device: $DEV_LOOP"
PART_MAP="/dev/mapper/`kpartx -v -a $DEV_LOOP | sed -r 's/add map ([a-z0-9]+).*/\1/'`"
echo "Partion mapped to $PART_MAP"
PART_LOOP=`losetup --find --show $PART_MAP`
echo "Partition loopback device: $PART_LOOP"
mkdosfs $PART_LOOP
mount $PART_LOOP /mnt
mkdir -p /mnt/boot/grub
echo "(hd0)	$DEV_LOOP" > /mnt/boot/grub/device.map
echo "(hd0,1)	$PART_LOOP" >> /mnt/boot/grub/device.map
sleep 2
echo "Installing GRUB"
grub-install --no-floppy --boot-directory=/mnt/boot $DEV_LOOP
umount /mnt
echo "Cleaning up"
losetup -d $PART_LOOP
kpartx -v -d $DEV_LOOP
losetup -d $DEV_LOOP
