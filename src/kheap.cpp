#include "kheap.h"

extern uint32_t end;
uint32_t heapTop = (uint32_t)&end;

constexpr uint32_t pageSize = 0x1000;
constexpr uint32_t pageMask = (uint32_t)-pageSize;

static uint32_t kmallocInternal(uint32_t size, uint32_t* physicalOut)
{
	if ((heapTop & pageMask) != 0)
	{
		heapTop &= pageMask;
		heapTop += pageSize;
	}
	
	if (physicalOut != nullptr)
	{
		*physicalOut = heapTop;
	}
	
	const uint32_t prevTop = heapTop;
	heapTop += size;
	return prevTop;
}

uint32_t kmallocPageAligned(uint32_t size)
{
	return kmallocInternal(size, nullptr);
}

uint32_t kmallocPageAlignedPhysical(uint32_t size, uint32_t* physicalOut)
{
	return kmallocInternal(size, physicalOut);
}

uint32_t kmallocAligned(uint32_t size, uint32_t alignment)
{
	if (heapTop % alignment != 0)
		heapTop += alignment - heapTop % alignment;
	
	return kmalloc(size);
}

uint32_t kmalloc(uint32_t size)
{
	const uint32_t prevTop = heapTop;
	heapTop += size;
	return prevTop;
}

