#include "Terminal.h"

size_t Terminal::row = 0;
size_t Terminal::column = 0;
uint16_t* const Terminal::buffer = VGA_BUFFER_ADDRESS;

void Terminal::scroll()
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

Terminal::Terminal()
	: color{makeColor(Color::LIGHT_GREY, Color::BLACK)}
{
}

void Terminal::clear()
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

void Terminal::newline()
{
	column = 0;
	if (++row == VGA_HEIGHT)
	{
		scroll();
		row--;
	}
	moveCursor();
}

void Terminal::putChar(char c)
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

void Terminal::writeString(const char* data)
{
	while (*data != 0)
	{
		putChar(*(data++));
	}
}

void Terminal::writeColoredString(const char* data)
{
	Color originalColor = getColor();
	while (*data != 0)
	{
		if (*data == '$' && *(data + 1) != 0)
		{
			data++;
			char c = *(data++);
			uint8_t ic = hexToInt(c);
			if (ic != 255)
				setColor(
					static_cast<Color>((
						static_cast<uint8_t>(originalColor) & 0xF0) | ic));
		}
		else
		{
			putChar(*(data++));
		}
	}
	setColor(originalColor);
}

void Terminal::writeDec(int32_t num)
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
	
	char buffer[10];
	size_t digits = 0;
	while (num > 0)
	{
		buffer[digits] = '0' + (char)(num % 10);
		num /= 10;
		digits++;
	}
	
	for (size_t i = digits; i > 0; --i)
	{
		putChar(buffer[i - 1]);
	}
}

void Terminal::writeHex(uint32_t num)
{
	if (num == 0)
	{
		putChar('0');
		return;
	}
	
	char buffer[8];
	size_t digits = 0;
	while (num > 0)
	{
		char nextDigit = (char)(num % 0x10);
		if (nextDigit < 10)
			buffer[digits] = '0' + nextDigit;
		else
			buffer[digits] = 'a' + nextDigit - 10;
			
		num >>= 4;
		++digits;
	}
	
	for (size_t i = digits; i > 0; --i)
	{
		putChar(buffer[i - 1]);
	}
}

void Terminal::moveCursor()
{
	uint16_t cursorLocation = row * 80 + column;
	outb(0x3D4, 14);
	outb(0x3D5, (cursorLocation >> 8) & 0xFF);
	outb(0x3D4, 15);
	outb(0x3D5, cursorLocation & 0xFF);
}

