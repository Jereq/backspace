#pragma once

#include <stddef.h>
#include <stdint.h>

struct registers
{
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t intNo, errCode;
	uint32_t eip, cs, eflags, useresp, ss; 
};

constexpr size_t IRQ0 = 32;
constexpr size_t IRQ1 = 33;
constexpr size_t IRQ2 = 34;
constexpr size_t IRQ3 = 35;
constexpr size_t IRQ4 = 36;
constexpr size_t IRQ5 = 37;
constexpr size_t IRQ6 = 38;
constexpr size_t IRQ7 = 39;
constexpr size_t IRQ8 = 40;
constexpr size_t IRQ9 = 41;
constexpr size_t IRQ10 = 42;
constexpr size_t IRQ11 = 43;
constexpr size_t IRQ12 = 44;
constexpr size_t IRQ13 = 45;
constexpr size_t IRQ14 = 46;
constexpr size_t IRQ15 = 47;

typedef void (*isr_t)(registers);
void registerInterruptHandler(uint8_t n, isr_t handler);

