#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#include "descriptorTables.h"
#include "isr.h"
#include "knew.h"
#include "OrderedArray.h"
#include "paging.h"
#include "Terminal.h"
#include "Timer.h"
#include "utility.h"

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
	
	initializePaging();
	
	term.writeColoredString("$7Hello $FMaster$7, $4welcome $7home!\n");
	
	asm volatile("sti");
	initTimer(100);
	
	OrderedArray<int, 8> array;
	array.insert(3);
	array.insert(1);
	array.insert(1234);
	array.insert(0);
	array.insert(1);
	
	term.writeString("Removing array[3] = ");
	term.writeDec(array[3]);
	term.putChar('\n');
	array.erase(array.begin() + 3);
	
	size_t count = 0;
	for (int i : array)
	{
		term.writeString("array[");
		term.writeDec(count++);
		term.writeString("] = ");
		term.writeDec(i);
		term.putChar('\n');
	}
	
	typedef OrderedArray<int, 8> Arr;
	char cArray[sizeof(Arr)];
	Arr* tArr = new (cArray) Arr;
	tArr->~Arr();
	
	for (;;);
}
LINKAGE_END

