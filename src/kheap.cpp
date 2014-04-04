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

void Heap::initHeap(uintptr_t start, uintptr_t end, uintptr_t max, bool supervisor, bool readOnly)
{
	startAddress = start;
	endAddress = end;
	maxAddress = max;
	this->supervisor = supervisor;
	this->readOnly = readOnly;
	
	AllocHeader* hole = (AllocHeader*)start;
	hole->size = endAddr - start - sizeof(AllocHeader) - sizeof(AllocFooter);
	hole->magic = heapMagicStart;
	hole->isHoe = true;
	index.insert(hole);
}

void* Heap::alloc(uint32_t size, bool pageAlign)
{
	const uint32_t decoratedSize = size + sizeof(AllocHeader) + sizeof(AllocFooter);
	index_t::iterator iterator = findSmallestHole(decoratedSize, pageAlign);
	if (iterator == index.end())
	{
	}
	
	
}

void Heap::free(void* p)
{
}

Heap::index_t::iterator Heap::findSmallestHole(uint32_t size, bool pageAlign)
{
	for (const AllocHeader*& header : index)
	{
		if (pageAlign)
		{
			const uintptr_t location = (uintptr_t)header;
			const uintptr_t holeStart = location + sizeof(AllocHeader);
			uintptr_t alignmentOffset = 0;
			if (holeStart & (pageSize - 1) != 0)
				alignmentOffset = pageSize - holeStart & (pageSize - 1);
			
			if (header->size - alignmentOffset >= size)
				return &header;
		}
		else if (header->size >= size)
			return &header;
	}
	
	return index.end();
}

void Heap::expand(uint32_t newSize)
{
	if (newSize & (pageSize - 1) != 0)
	{
		newSize &= pageMask;
		newSize += pageSize;
	}
	
	const uint32_t oldSize = endAddress - startAddress;
	uint32_t i = oldSize;
	while (i < newSize)
	{
		allocFrame(getPage(startAddress + i, true, kernelDirectory), supervisor, readOnly);
		i += pageSize;
	}
	
	endAddress = startAddress + newSize;
}

uint32_t Heap::contract(uint32_t newSize)
{
	if (newSize & (pageSize - 1) != 0)
	{
		newSize &= pageMask;
		newSize += pageSize;
	}
	
	if (newSize < heapMinSize)
		newSize = heapMinSize;
	
	const uint32_t oldSize = endAddress - startAddress;
	uint32_t i = oldSize - pageSize;
	while (newSize < i)
	{
		freeFrame(getPage(startAddress + i, false, kernelDirectory));
		i -= pageSize;
	}
	
	endAddress = startAddress + newSize;
	return newSize;
}

