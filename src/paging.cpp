#include "paging.h"

#include "errorHandling.h"
#include "kheap.h"
#include "Terminal.h"
#include "utility.h"

PageDirectory* kernelDirectory = nullptr;
PageDirectory* currentDirectory = nullptr;

uint32_t* frames;
uint32_t nFrames;

extern uint32_t heapTop;

static constexpr uint32_t frameSize = 0x1000;
static constexpr uint32_t bitsPerWord = sizeof(uint32_t) * 8;

static constexpr uint32_t indexFromBit(uint32_t bit)
{
	return bit / bitsPerWord;
}

static constexpr uint32_t offsetFromBit(uint32_t bit)
{
	return bit % bitsPerWord;
}

static void setFrame(uint32_t frameAddr)
{
	const uint32_t frame = frameAddr / frameSize;
	const uint32_t idx = indexFromBit(frame);
	const uint32_t off = offsetFromBit(frame);
	frames[idx] |= (1 << off);
}

/*static void clearFrame(uint32_t frameAddr)
{
	const uint32_t frame = frameAddr / frameSize;
	const uint32_t idx = indexFromBit(frame);
	const uint32_t off = offsetFromBit(frame);
	frames[idx] &= ~(1 << off);
}*/

/*static bool testFrame(uint32_t frameAddr)
{
	const uint32_t frame = frameAddr / frameSize;
	const uint32_t idx = indexFromBit(frame);
	const uint32_t off = offsetFromBit(frame);
	return (frames[idx] & (1 << off)) != 0;
}*/

static uint32_t findFirstFreeFrame()
{
	for (uint32_t i = 0; i < indexFromBit(nFrames); ++i)
	{
		if (frames[i] == 0xFFFFFFFF)
			continue;
			
		for (uint32_t j = 0; j < bitsPerWord; ++j)
		{
			const uint32_t toTest = 1 << j;
			
			if ((frames[i] & toTest) == 0)
				return i * bitsPerWord + j;
		}
	}
	
	return (uint32_t)-1;
}

static void allocFrame(Page* page, bool isKernel, bool isWritable)
{
	if (page->frame != 0)
		return;
	
	const uint32_t idx = findFirstFreeFrame();
	if (idx == (uint32_t)-1)
	{
		PANIC("No free frames!");
	}
	
	setFrame(idx * frameSize);
	page->present = 1;
	page->writable = isWritable ? 1 : 0;
	page->userMode = isKernel ? 0 : 1;
	page->frame = idx;
}

/*static void freeFrame(Page* page)
{
	uint32_t frame = page->frame;
	if (frame == 0)
		return;
		
	clearFrame(frame);
	page->frame = 0;
}*/

void initializePaging()
{
	constexpr uint32_t memoryEndPage = 0x1000000;
	
	nFrames = memoryEndPage / frameSize;
	frames = (uint32_t*)kmalloc(indexFromBit(nFrames));
	memset(frames, 0, indexFromBit(nFrames));
	
	kernelDirectory = (PageDirectory*)kmallocPageAligned(sizeof(PageDirectory));
	memset(kernelDirectory, 0, sizeof(PageDirectory));
	currentDirectory = kernelDirectory;
	
	unsigned int i = 0;
	while (i < heapTop)
	{
		allocFrame(getPage(i, true, kernelDirectory), false, false);
		i += frameSize;
	}
	
	registerInterruptHandler(14, pageFault);
	
	switchPageDirectory(kernelDirectory);
}

void switchPageDirectory(PageDirectory* newDir)
{
	currentDirectory = newDir;
	asm volatile("mov %0, %%cr3":: "r"(&newDir->tablesPhysical));
	uint32_t cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

Page* getPage(uint32_t address, bool createIfMissing, PageDirectory* directory)
{
	address /= frameSize;
	
	const uint32_t tableIdx = address / 1024;
	if (directory->tables[tableIdx])
		return &directory->tables[tableIdx]->pages[address % 1024];
	
	if (createIfMissing)
	{
		uint32_t physAddr;
		directory->tables[tableIdx] = (PageTable*)kmallocPageAlignedPhysical(sizeof(PageTable), &physAddr);
		memset(directory->tables[tableIdx], 0, frameSize);
		directory->tablesPhysical[tableIdx] = physAddr | 7;
		return &directory->tables[tableIdx]->pages[address % 1024];
	}
	
	return 0;
}

void pageFault(registers regs)
{
	uint32_t faultingAddress;
	asm volatile("mov %% cr2, %0" : "=r" (faultingAddress));
	
	bool present = (regs.errCode & 0x1) == 0;
	bool writing = (regs.errCode & 0x2) != 0;
	bool userMode = (regs.errCode & 0x4) != 0;
	bool reserved = (regs.errCode & 0x8) != 0;
	//bool id = (regs.errCode & 0x10) != 0;
	
	Terminal term;
	term.writeString("Page fault! ( ");
	if (present) term.writeString("present ");
	if (writing) term.writeString("read-only ");
	if (userMode) term.writeString("user-mode ");
	if (reserved) term.writeString("reserved ");
	term.writeString(") at 0x");
	term.writeHex(faultingAddress);
	term.putChar('\n');
	PANIC("Page fault");
}

