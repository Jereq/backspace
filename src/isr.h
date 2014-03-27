#pragma once

struct registers
{
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t intNo, errCode;
	uint32_t eip, cs, eflags, useresp, ss; 
};
