#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#include "utility.h"
#include "descriptorTables.h"
#include "Terminal.h"

void insertNum(int num, char* begin, char* end)
{
	if (begin >= end)
	{
		return;
	}
	
	if (num < 0)
	{
		*(begin++) = '-';
		num = -num;
	}
	else if (num == 0)
	{
		*begin = '0';
		return;
	}
	
	int count = 0;
	int tmpNum = num;
	while (tmpNum != 0)
	{
		tmpNum /= 10;
		count++;
	}
	
	if (count > end - begin)
	{
		return;
	}
	
	for (int i = count - 1; i >= 0; i--)
	{
		const char digit = num % 10;
		num /= 10;
		
		begin[i] = '0' + digit;
	}
}

LINKAGE_BEGIN
void kernel_early_main()
{
	// Init kernel space
}

void kernel_main()
{
	initDescriptorTables();

	Terminal term;
	term.clear();
	term.writeString("Hello, kernel World!\nNext line\n");
	term.writeString("Second call\n");
	term.writeColoredString("$7Hello $FMaster$7, $4welcome $7home!\n");
	
	asm volatile ("int $0x3");
	asm volatile ("int $0x4");
}
LINKAGE_END

