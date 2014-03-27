#include "isr.h"

#include "common.h"
#include "Terminal.h"
#include "utility.h"

static isr_t interruptHandlers[256];

LINKAGE_BEGIN

void isrHandler(registers regs)
{
	Terminal term;
	term.writeString("received interrupt: ");
	term.writeDec(regs.intNo);
	term.putChar('\n');
}

void irqHandler(registers regs)
{
	if (regs.intNo >= 40)
	{
		outb(0xA0, 0x20);
	}
	
	outb(0x20, 0x20);
	
	if (interruptHandlers[regs.intNo] != 0)
	{
		isr_t handler  = interruptHandlers[regs.intNo];
		handler(regs);
	}
}

LINKAGE_END

void registerInterruptHandler(uint8_t n, isr_t handler)
{
	interruptHandlers[n] = handler;
}

