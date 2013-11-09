all: backspace.bin

boot.o: boot.s
	i586-elf-as boot.s -o boot.o

kernel.o: kernel.cpp
	i586-elf-g++ -c kernel.cpp -o kernel.o -std=gnu++0x -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

backspace.bin: linker.ld boot.o kernel.o
	i586-elf-gcc -T linker.ld -o backspace.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

disk.img: createGRUB_Image.sh
	./createGRUB_Image.sh
	
install: disk.img backspace.bin grub.cfg installToShared.sh
	./installToShared.sh
	
clean;
	rm boot.o
	rm kernel.o
	rm disk.img
