#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

enum class VGA_Color : uint8_t
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

uint8_t makeColor(VGA_Color foreground, VGA_Color background)
{
	return (uint8_t)foreground | (uint8_t)background << 4;
}

uint16_t makeVGA_Entry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
};

size_t strlen(const char* str)
{
	size_t ret = 0;
	while (str[ret] != 0)
		ret++;
	return ret;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;
static uint16_t* const VGA_BUFFER_ADDRESS = (uint16_t*)0xB8000;

size_t terminalRow;
size_t terminalColumn;
uint8_t terminalColor;
uint16_t* terminalBuffer;

void terminalInitialize()
{
	terminalRow = 0;
	terminalColumn = 0;
	terminalColor = makeColor(VGA_Color::LIGHT_GREY, VGA_Color::BLACK);
	terminalBuffer = VGA_BUFFER_ADDRESS;
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminalBuffer[index] = makeVGA_Entry(' ', terminalColor);
		}
	}
}

void terminalSetColor(uint8_t color)
{
	terminalColor = color;
}

void terminalPutEntryAt(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminalBuffer[index] = makeVGA_Entry(c, color);
}

void terminalPutChar(char c)
{
	terminalPutEntryAt(c, terminalColor, terminalColumn, terminalRow);
	if (++terminalColumn == VGA_WIDTH)
	{
		terminalColumn = 0;
		if (++terminalRow == VGA_HEIGHT)
		{
			terminalRow = 0;
		}
	}
}

void terminalWriteString(const char* data)
{
	size_t datalen = strlen(data);
	for (size_t i = 0; i < datalen; i++)
	{
		terminalPutChar(data[i]);
	}
}

#if defined(__cplusplus)
extern "C"
#endif
void kernel_main()
{
	terminalInitialize();
	terminalWriteString("Hello, kernel World!\n");
}

