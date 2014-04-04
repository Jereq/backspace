#pragma once

#include <cstdint>

#include "OrderedArray.h"

uint32_t kmallocPageAligned(uint32_t size);
uint32_t kmallocPageAlignedPhysical(uint32_t size, uint32_t* physicalOut);
uint32_t kmallocAligned(uint32_t size, uint32_t alignment);	// custom aligned
uint32_t kmalloc(uint32_t size);	// non-aligned

constexpr uintptr_t kheapStart = 0xc0000000;
constexpr uint32_t kheapInitialSize = 0x100000;
constexpr uint32_t heapIndexSize = 0x20000;
constexpr uint32_t heapMagicStart = 0xfeedbeef;
constexpr uint32_t heapMagicEnd = 0xfee7beef;
constexpr uint32_t heapMinSize = 0x70000;

struct AllocHeader
{
	uint32_t magic;
	bool isHole;
	uint32_t size;
	
	bool operator<(const AllocHeader& right) const
	{
		return size < right.size;
	};
};

struct AllocFooter
{
	uint32_t magic;
	AllocHeader* header;
};

struct Heap
{
	typedef OrderedArray<AllocHeader*, heapIndexSize> index_t;
	index_t index;
	uintptr_t startAddress;
	uintptr_t endAddress;
	uintptr_t maxAddress;
	bool supervisor;
	bool readOnly;
	
	void initHeap(uintptr_t start, uintptr_t end, uintptr_t max, bool supervisor, bool readOnly);
	
	void* alloc(uint32_t size, bool pageAlign);
	void free(void* p);
	
private:
	index_t::iterator findSmallestHole(uint32_t size, bool pageAlign);
	void expand(uint32_t newSize);
	uint32_t contract(uint32_t newSize);
};

