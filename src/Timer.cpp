#include "Timer.h"

#include "isr.h"
#include "Terminal.h"

static uint32_t tick = 0;

static void timerCallback(registers)
{
	++tick;
	
	if (tick % 100 == 0)
	{
		Terminal term;
		term.writeString("Tick: ");
		term.writeDec(tick);
		term.putChar('\n');
	}
}

void initTimer(uint32_t frequency)
{
	registerInterruptHandler(IRQ0, &timerCallback);
	
	uint16_t divisor = 1193180 / frequency;
	
	outb(0x43, 0x36);
	
	uint8_t l = (uint8_t)(divisor & 0xFF);
	uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);
	
	outb(0x40, l);
	outb(0x40, h);
}

