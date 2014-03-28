#pragma once

#include "common.h"
#include "isr.h"

struct Page
{
	uint32_t present	: 1;
	uint32_t writable	: 1;
	uint32_t userMode	: 1;
	uint32_t accessed	: 1;
	uint32_t dirty		: 1;
	uint32_t unused		: 7;
	uint32_t frame		: 20;
};

struct PageTable
{
	Page pages[1024];
};

struct PageDirectory
{
	PageTable* tables[1024];
	
	uint32_t tablesPhysical[1024];
	
	uint32_t physicalAddr;
};

void initializePaging();

void switchPageDirectory(PageDirectory* newDir);

Page* getPage(uint32_t address, bool createIfMissing, PageDirectory* directory);

void pageFault(registers regs);

