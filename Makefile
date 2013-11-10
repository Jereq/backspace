COMP:=i586-elf-g++
COMP_FLAGS:=-std=gnu++0x -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
OBJS:=boot.o kernel.o descriptorTables.o gdt.o interrupt.o isr.o
AS_FLAGS:=-f elf

all: backspace.bin

.s.o:
	i586-elf-as $< -o $@
	
.cpp.o:
	$(COMP) -c $< -o $@ $(COMP_FLAGS)
	
gdt.o: gdt.s
	nasm $(AS_FLAGS) gdt.s -o gdt.o
	
interrupt.o: interrupt.s
	nasm $(AS_FLAGS) interrupt.s -o interrupt.o

kernel.o: kernel.cpp Terminal.h common.h
	$(COMP) -c kernel.cpp -o kernel.o $(COMP_FLAGS)

CRTI_OBJ:=crti.o
CRTBEGIN_OBJ:=$(shell $(COMP) $(COMP_FLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(COMP) $(COMP_FLAGS) -print-file-name=crtend.o)
CRTN_OBJ=crtn.o

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
INTERNAL_OBJS:=$(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)

backspace.bin: linker.ld $(OBJ_LINK_LIST)
	$(COMP) -T linker.ld -o backspace.bin -ffreestanding -O2 -nostdlib $(OBJ_LINK_LIST) -lgcc

disk.img: createGRUB_Image.sh
	./createGRUB_Image.sh
	
install: disk.img backspace.bin grub.cfg installToShared.sh
	./installToShared.sh
	
clean:
	rm -f disk.img kernel.bin $(INTERNAL_OBJS)
