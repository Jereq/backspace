#!/bin/sh
TMP_IMAGE=`mktemp`
BUILD_DIR=build
IMAGE=$BUILD_DIR/disk.img
BIN_DIR=$BUILD_DIR/bin
KERNEL=$BIN_DIR/backspace.bin
DESTINATION=/media/sf_Shared_Folder/disk.img

echo "Created temporary file $TMP_IMAGE"
cp $IMAGE $TMP_IMAGE
DEV_LOOP=`losetup --find --show $TMP_IMAGE`
echo "Image loopback device: $DEV_LOOP"
PART_MAP="/dev/mapper/`kpartx -v -a $DEV_LOOP | sed -r 's/add map ([a-z0-9]+).*/\1/'`"
echo "Partion mapped to $PART_MAP"
PART_LOOP=`losetup --find --show $PART_MAP`
echo "Partition loopback device: $PART_LOOP"
mount $PART_LOOP /mnt
cp $KERNEL /mnt/boot/
cp grub.cfg /mnt/boot/grub/
umount /mnt
losetup -d $PART_LOOP
kpartx -v -d $DEV_LOOP
losetup -d $DEV_LOOP
mv $TMP_IMAGE $DESTINATION
