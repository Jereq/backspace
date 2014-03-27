# Setup compilation variables
AS:=i586-elf-as
COMP:=i586-elf-g++
COMP_FLAGS:=-std=gnu++0x -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib
AS_FLAGS:=-f elf

LIBS:=-lgcc

# Setup folder structure
SRC_DIR:=src
BUILD_DIR:=build
BIN_DIR:=$(BUILD_DIR)/bin

# Objects, relative to source directory
OBJS:=\
boot.o \
kernel.o \
descriptorTables.o \
gdt.o \
interrupt.o \
isr.o \
Terminal.o \
Timer.o \

# Alter paths to be in source directory
OBJS:=$(addprefix $(BUILD_DIR)/,$(OBJS))

# Default build target
all: $(BIN_DIR)/backspace.bin

# Prevent search for badly names files
.PHONEY: all clean install

# Generic assably rule
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	$(AS) $< -o $@

# Generic cpp rule
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(COMP) -c $< -o $@ $(COMP_FLAGS)

# This one was found in another syntax, handle seperately for now
$(BUILD_DIR)/gdt.o: $(SRC_DIR)/gdt.s
	nasm $(AS_FLAGS) $< -o $@

# This one too
$(BUILD_DIR)/interrupt.o: $(SRC_DIR)/interrupt.s
	nasm $(AS_FLAGS) $< -o $@

# Add dependency on headers
COMMON_H:=$(SRC_DIR)/common.h
ISR_H:=$(SRC_DIR)/isr.h
TERMINAL_H:=$(SRC_DIR)/Terminal.h $(COMMON_H)
TIMER_H:=$(SRC_DIR)/Timer.h
UTILITY_H:=$(SRC_DIR)/utility.h
DESCRIPTOR_TABLES_H:=$(SRC_DIR)/descriptorTables.h $(UTILITY_H)

$(BUILD_DIR)/descriptorTables.o : $(DESCRIPTOR_TABLES_H) $(COMMON_H) $(ISR_H) $(UTILITY_H)
$(BUILD_DIR)/isr.o : $(ISR_H) $(COMMON_H) $(TERMINAL_H) $(UTILITY_H)
$(BUILD_DIR)/kernel.o : $(DESCRIPTOR_TABLES_H) $(ISR_H) $(TERMINAL_H) $(TIMER_H) $(UTILITY_H)
$(BUILD_DIR)/Terminal.o : $(TERMINAL_H)
$(BUILD_DIR)/Timer.o : $(TIMER_H) $(ISR_H) $(TERMINAL_H)

# Add cpp runtime environement supporting files
CRTI_OBJ:=$(BUILD_DIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(COMP) $(COMP_FLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(COMP) $(COMP_FLAGS) -print-file-name=crtend.o)
CRTN_OBJ=$(BUILD_DIR)/crtn.o

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
INTERNAL_OBJS:=$(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)

# Create folder if not exising
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Main target, this is the binary kernel see
$(BIN_DIR)/backspace.bin: $(BUILD_DIR) $(BIN_DIR) linker.ld $(OBJ_LINK_LIST)
	$(COMP) -T linker.ld -o $@ $(COMP_FLAGS) $(OBJ_LINK_LIST) $(LIBS)

# Run script to create a bootable disk image, not fun to do in make
$(BUILD_DIR)/disk.img: createGRUB_Image.sh
	./createGRUB_Image.sh

# Run script to install kernel binary into bootable disk image
install: $(BUILD_DIR)/disk.img $(BIN_DIR)/backspace.bin grub.cfg installToShared.sh
	./installToShared.sh

# Remove any recreatable files	
clean:
	rm -f $(BUILD_DIR)/disk.img $(BIN_DIR)/backspace.bin $(INTERNAL_OBJS)

