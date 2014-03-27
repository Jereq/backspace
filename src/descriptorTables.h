#pragma once

#include <stdint.h>
#include "utility.h"

LINKAGE_BEGIN
#pragma pack(push, 1)
struct GDT_Entry
{
	uint16_t limitLow;
	uint16_t baseLow;
	uint8_t baseMiddle;
	uint8_t access;
	uint8_t granularity;
	uint8_t baseHigh;
};

struct GDT_Ptr
{
	uint16_t limit;
	uint32_t base;
};

void initDescriptorTables();

struct IDT_Entry
{
	uint16_t baseLow;
	uint16_t sel;
	uint8_t always0;
	uint8_t flags;
	uint16_t baseHigh;
};

struct IDT_Ptr
{
	uint16_t limit;
	uint32_t base;
};

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#pragma pack(pop)
LINKAGE_END

