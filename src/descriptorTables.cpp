#include "descriptorTables.h"

#include "common.h"
#include "utility.h"
LINKAGE_BEGIN
extern void gdtFlush(uint32_t);
extern void idtFlush(uint32_t);

static void initGDT();
static void initIDT();
static void gdtSetGate(int32_t, uint32_t, uint32_t, uint8_t, uint8_t);
static void idtSetGate(uint8_t, uint32_t, uint16_t, uint8_t);

GDT_Entry gdt_entries[5];
GDT_Ptr gdt_ptr;
IDT_Entry idt_entries[256];
IDT_Ptr idt_ptr;

void initDescriptorTables()
{
	initGDT();
	initIDT();
}

static void initGDT()
{
	gdt_ptr.limit = sizeof(gdt_entries) - 1;
	gdt_ptr.base = (uint32_t)&gdt_entries;
	
	gdtSetGate(0, 0, 0, 0, 0);
	gdtSetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdtSetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdtSetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdtSetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	
	gdtFlush((uint32_t)&gdt_ptr);
}

static void initIDT()
{
	idt_ptr.limit = sizeof(idt_entries) - 1;
	idt_ptr.base = (uint32_t)&idt_entries;
	
	memset(&idt_entries, 0, sizeof(idt_entries));
	
	idtSetGate(0, (uint32_t)isr0, 0x08, 0x8E);
	idtSetGate(1, (uint32_t)isr1, 0x08, 0x8E);
	idtSetGate(2, (uint32_t)isr2, 0x08, 0x8E);
	idtSetGate(3, (uint32_t)isr3, 0x08, 0x8E);
	idtSetGate(4, (uint32_t)isr4, 0x08, 0x8E);
	idtSetGate(5, (uint32_t)isr5, 0x08, 0x8E);
	idtSetGate(6, (uint32_t)isr6, 0x08, 0x8E);
	idtSetGate(7, (uint32_t)isr7, 0x08, 0x8E);
	idtSetGate(8, (uint32_t)isr8, 0x08, 0x8E);
	idtSetGate(9, (uint32_t)isr9, 0x08, 0x8E);
	idtSetGate(10, (uint32_t)isr10, 0x08, 0x8E);
	idtSetGate(11, (uint32_t)isr11, 0x08, 0x8E);
	idtSetGate(12, (uint32_t)isr12, 0x08, 0x8E);
	idtSetGate(13, (uint32_t)isr13, 0x08, 0x8E);
	idtSetGate(14, (uint32_t)isr14, 0x08, 0x8E);
	idtSetGate(15, (uint32_t)isr15, 0x08, 0x8E);
	idtSetGate(16, (uint32_t)isr16, 0x08, 0x8E);
	idtSetGate(17, (uint32_t)isr17, 0x08, 0x8E);
	idtSetGate(18, (uint32_t)isr18, 0x08, 0x8E);
	idtSetGate(19, (uint32_t)isr19, 0x08, 0x8E);
	idtSetGate(20, (uint32_t)isr20, 0x08, 0x8E);
	idtSetGate(21, (uint32_t)isr21, 0x08, 0x8E);
	idtSetGate(22, (uint32_t)isr22, 0x08, 0x8E);
	idtSetGate(23, (uint32_t)isr23, 0x08, 0x8E);
	idtSetGate(24, (uint32_t)isr24, 0x08, 0x8E);
	idtSetGate(25, (uint32_t)isr25, 0x08, 0x8E);
	idtSetGate(26, (uint32_t)isr26, 0x08, 0x8E);
	idtSetGate(27, (uint32_t)isr27, 0x08, 0x8E);
	idtSetGate(28, (uint32_t)isr28, 0x08, 0x8E);
	idtSetGate(29, (uint32_t)isr29, 0x08, 0x8E);
	idtSetGate(30, (uint32_t)isr30, 0x08, 0x8E);
	idtSetGate(31, (uint32_t)isr31, 0x08, 0x8E);
	
	idtFlush((uint32_t)&idt_ptr);
}

static void gdtSetGate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries[num].baseLow = (base & 0xFFFF);
	gdt_entries[num].baseMiddle = (base >> 16) & 0xFF;
	gdt_entries[num].baseHigh = (base >> 24) & 0xFF;
	
	gdt_entries[num].limitLow = (limit & 0xFFFF);
	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access = access;
}

static void idtSetGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt_entries[num].baseLow = base & 0xFFFF;
	idt_entries[num].baseHigh = (base >> 16) & 0xFFFF;
	
	idt_entries[num].sel = sel;
	idt_entries[num].always0 = 0;
	// We must uncomment the OR below when we get to using user-mode.
	// It sets the interrupt gate's priviledge level to 3.
	idt_entries[num].flags = flags /* | 0x60 */;
}

LINKAGE_END

