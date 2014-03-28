#pragma once

#include <stdint.h>

#define PANIC(msg) panic(msg, __LINE__, __FILE__)

void panic(const char* msg, uint32_t line, const char* file);

