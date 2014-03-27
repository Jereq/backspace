#pragma once

#if defined(__cplusplus)
#define LINKAGE_BEGIN extern "C" {
#define LINKAGE_END }
#else
#define LINKAGE_BEGIN
#define LINAKGE_END
#endif

#include <stddef.h>

inline void memset(void* ptr, int value, size_t num)
{
	const unsigned char c_value = value;
	unsigned char* const c_ptr = (unsigned char*)ptr;
	for (size_t i = 0; i < num; i++)
	{
		c_ptr[i] = c_value;
	}
}
