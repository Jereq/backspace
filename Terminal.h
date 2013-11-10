#pragma once

#include "common.h"
#include <stddef.h>

class Terminal
{
private:
	constexpr static size_t VGA_WIDTH = 80;
	constexpr static size_t VGA_HEIGHT = 25;
	constexpr static uint16_t* VGA_BUFFER_ADDRESS = (uint16_t*)0xB8000;
	
	size_t row;
	size_t column;
	uint8_t color;
	uint16_t* buffer;
	
	void scroll();

	uint8_t hexToInt(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		else if (c >= 'a' && c <= 'f')
			return c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			return c - 'A' + 10;
		else
			return 255;
	}
	
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
	
	Terminal();

	void clear();

	uint8_t makeColor(Color foreground, Color background)
	{
		return (uint8_t)foreground | (uint8_t)background << 4;
	}

	uint16_t makeEntry(char c, uint8_t color)
	{
		uint16_t c16 = c;
		uint16_t color16 = color;
		return c16 | color16 << 8;
	};
	
	void setColor(uint8_t color)
	{
		this->color = color;
	}

	uint8_t getColor()
	{
		return color;
	}

	void putEntryAt(char c, uint8_t color, size_t x, size_t y)
	{
		const size_t index = y * VGA_WIDTH + x;
		buffer[index] = makeEntry(c, color);
	}

	void newline()
	{
		column = 0;
		if (++row == VGA_HEIGHT)
		{
			scroll();
			row--;
		}
		moveCursor();
	}

	void putChar(char c)
	{
		if (c == '\n')
		{
			newline();
			return;
		}
	
		putEntryAt(c, color, column, row);
		if (++column == VGA_WIDTH)
		{
			newline();
		}
		
		moveCursor();
	}

	void writeString(const char* data)
	{
		while (*data != 0)
		{
			putChar(*(data++));
		}
	}

	void writeColoredString(const char* data)
	{
		uint8_t originalColor = getColor();
		while (*data != 0)
		{
			if (*data == '$' && *(data + 1) != 0)
			{
				data++;
				char c = *(data++);
				uint8_t ic = hexToInt(c);
				if (ic != 255)
					setColor((originalColor & 0xF0) | ic);
			}
			else
			{
				putChar(*(data++));
			}
		}
		setColor(originalColor);
	}
	
	void writeDec(int32_t num)
	{
		if (num == 0)
		{
			putChar('0');
			return;
		}
		
		if (num < 0)
		{
			putChar('-');
			num = -num;
		}
		
		char buffer[31];
		size_t digits = 0;
		while (num > 0)
		{
			buffer[digits] = '0' + (char)(num % 10);
			num /= 10;
			digits++;
		}
		
		for (size_t i = digits; i > 0; i++)
		{
			putChar(buffer[i - 1]);
		}
	}
	
	void moveCursor();
};

inline Terminal::Terminal()
	: row{0},
	  column{0},
	  color{makeColor(Color::LIGHT_GREY, Color::BLACK)},
	  buffer{VGA_BUFFER_ADDRESS}
{
}

inline void Terminal::clear()
{
	const uint16_t clearVal = makeEntry(' ', color);
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			buffer[index] = clearVal;
		}
	}
	moveCursor();
}

inline void Terminal::scroll()
{
	for (size_t i = 0; i < VGA_HEIGHT - 1; i++)
	{
		const size_t rowOffset = i * VGA_WIDTH;
		for (size_t j = 0; j < VGA_WIDTH; j++)
		{
			const size_t index = rowOffset + j;
			buffer[index] = buffer[index + VGA_WIDTH];
		}
	}

	const size_t lastRowOffset = VGA_WIDTH * (VGA_HEIGHT - 1);
	const uint16_t clearEntry = makeEntry(' ', color);
	for (size_t i = 0; i < VGA_WIDTH; i++)
	{
		buffer[lastRowOffset + i] = clearEntry;
	}
}

inline void Terminal::moveCursor()
{
	uint16_t cursorLocation = row * 80 + column;
	outb(0x3D4, 14);
	outb(0x3D5, (cursorLocation >> 8) & 0xFF);
	outb(0x3D4, 15);
	outb(0x3D5, cursorLocation & 0xFF);
}
