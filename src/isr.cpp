#include "isr.h"

#include "common.h"
#include "Terminal.h"
#include "utility.h"

static isr_t interruptHandlers[256];

LINKAGE_BEGIN

void isrHandler(registers regs)
{
	const isr_t interruptHandler = interruptHandlers[regs.intNo];
	if (interruptHandler != nullptr)
	{
		interruptHandler(regs);
		return;
	}

	Terminal term;
	term.writeString("unhandled interrupt: ");
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
	
	const isr_t interruptHandler = interruptHandlers[regs.intNo];
	if (interruptHandler != nullptr)
	{
		interruptHandler(regs);
		return;
	}
}

LINKAGE_END

void clearInterruptHandlers()
{
	memset(interruptHandlers, 0, sizeof(interruptHandlers));
}

void registerInterruptHandler(uint8_t n, isr_t handler)
{
	interruptHandlers[n] = handler;
}

