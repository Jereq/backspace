#pragma once

#include <stdint.h>

uint32_t kmallocPageAligned(uint32_t size);
uint32_t kmallocPageAlignedPhysical(uint32_t size, uint32_t* physicalOut);
uint32_t kmallocAligned(uint32_t size, uint32_t alignment);	// custom aligned
uint32_t kmalloc(uint32_t size);	// non-aligned

