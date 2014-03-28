#include "errorHandling.h"

#include "Terminal.h"

void panic(const char* msg, uint32_t line, const char* file)
{
	asm volatile("cli");
	
	Terminal term;
	term.writeString(file);
	term.putChar('(');
	term.writeDec(line);
	term.writeString("): PANIC: ");
	term.writeString(msg);
	
	for (;;) asm volatile("hlt");
}

