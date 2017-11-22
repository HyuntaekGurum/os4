.PHONY: all run debug clean setup kernel

all: disk.img

setup:
	make -C setup

kernel:
	make -C kernel

disk.img: scripts/grub.cfg setup kernel
	dd if=/dev/zero of=disk.img bs=512 count=131072
	sfdisk disk.img < scripts/disk.layout
	sudo losetup /dev/loop0 disk.img
	sudo losetup /dev/loop1 disk.img -o 1048576
	sudo mkfs.ext2 /dev/loop1
	sudo mount /dev/loop1 /mnt
	sudo grub-install --root-directory=/mnt --modules="normal ext2 multiboot2 part_msdos" /dev/loop0
	sudo cp scripts/grub.cfg /mnt/boot/grub
	sudo cp setup/setup.elf /mnt/boot
	sudo cp kernel/kernel.bin /mnt/boot
	sudo umount /mnt
	sudo losetup -D

run: disk.img
	qemu-system-x86_64 -hda disk.img

debug: disk.img
	qemu-system-x86_64 -hda disk.img -monitor stdio

clean:
	make -C setup clean
	make -C kernel clean
	rm -f disk.img
