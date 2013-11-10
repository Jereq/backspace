#include "common.h"
#include "isr.h"
#include "Terminal.h"
#include "utility.h"

LINKAGE_BEGIN

void isrHandler(registers regs)
{
	Terminal term;
	term.writeString("received interrupt: ");
	term.writeDec(regs.intNo);
	term.putChar('\n');
}

LINKAGE_END

