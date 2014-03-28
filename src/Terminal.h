#pragma once

#include "common.h"
#include <stddef.h>

class Terminal
{
public:
	enum class Color : uint8_t
	{
		BLACK = 0,
		BLUE = 1,
		GREEN = 2,
		CYAN = 3,
		RED = 4,
		MAGENTA = 5,
		BROWN = 6,
		LIGHT_GREY = 7,
		DARK_GREY = 8,
		LIGHT_BLUE = 9,
		LIGHT_GREEN = 10,
		LIGHT_CYAN = 11,
		LIGHT_RED = 12,
		LIGHT_MAGENTA = 13,
		LIGHT_BROWN = 14,
		WHITE = 15,
	};

private:
	static constexpr size_t VGA_WIDTH = 80;
	static constexpr size_t VGA_HEIGHT = 25;
	static constexpr uint16_t* VGA_BUFFER_ADDRESS = (uint16_t*)0xB8000;
	
	static size_t row;
	static size_t column;
	static uint16_t* const buffer;
	
	uint8_t color;
	
	void scroll();

	static constexpr uint8_t makeColor(Color foreground, Color background)
	{
		return (uint8_t)foreground | (uint8_t)background << 4;
	}

	static constexpr uint8_t hexToInt(char c)
	{
		return (c >= '0' && c <= '9') ? c - '0' :
			(c >= 'a' && c <= 'f') ? c - 'a' + 10 :
			(c >= 'A' && c <= 'F') ? c - 'A' + 10 :
			255;
	}

	static constexpr uint16_t makeEntry(char c, uint8_t color)
	{
		return static_cast<uint16_t>(c) | static_cast<uint16_t>(color) << 8;
	};

	static void putEntryAt(char c, uint8_t color, size_t x, size_t y)
	{
		const size_t index = y * VGA_WIDTH + x;
		buffer[index] = makeEntry(c, color);
	}
	
public:
	Terminal();

	void clear();
	
	void setColor(Color color)
	{
		this->color = static_cast<uint8_t>(color);
	}

	Color getColor()
	{
		return static_cast<Color>(color);
	}

	void newline();
	void putChar(char c);
	void writeString(const char* data);
	void writeColoredString(const char* data);
	void writeDec(int32_t num);
	void writeHex(uint32_t num);
	
	static void moveCursor();
};

