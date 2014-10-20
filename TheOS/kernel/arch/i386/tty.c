#include <kernel/screen/tty.h>
#include <kernel/system.h>

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint8_t terminal_header_color;
uint16_t* terminal_buffer;
uint16_t* terminal_header_buffer;

void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = STATUS_NORMAL;
	terminal_header_color = make_color(COLOR_WHITE, COLOR_GREY);
	terminal_buffer = VGA_MEMORY;
	terminal_header_buffer = (uint16_t *) 0xB8000;
	terminal_clear();
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_setcursor() {
	const size_t index = (terminal_row + 1) * VGA_WIDTH + terminal_column;
	uint8_t low = index & 0xff;
	uint8_t high = index >> 8;

	// set the low byte
	outb(0x3d4, 0x0f);	// tell the controller what we want to do
	outb(0x3d5, low);

	// set the high byte
	outb(0x3d4, 0x0e);
	outb(0x3d5, high);
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_clear() {

	// clear the header
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		terminal_header_buffer[x] = make_vgaentry(' ', terminal_header_color);
	}

	// clear the screen
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}

	terminal_row = 0;
	terminal_column = 0;
	terminal_setcursor();
}


void terminal_scroll() {
	char *from = (char *)(VGA_MEMORY + VGA_WIDTH);
	size_t size = (VGA_HEIGHT - 1) * VGA_WIDTH * 2;

	memmove(VGA_MEMORY, from, size);

	terminal_row--;

	for (uint32_t i = 0; i < VGA_WIDTH; i++) {
		terminal_putentryat(' ', terminal_color, i, terminal_row);
	}

	terminal_setcursor();
}

static void terminal_header_print(const char* data, size_t data_length, int index) {
	for (size_t i = 0; i < data_length && i < VGA_WIDTH; i++) {
		terminal_header_buffer[i + index] = make_vgaentry(data[i], terminal_header_color);
	}
	if (data_length + index < VGA_WIDTH) {
		for (uint32_t i = data_length + index; i < VGA_WIDTH; i++) {
			terminal_header_buffer[i] = make_vgaentry(' ', terminal_header_color);
		}
	}
}

void terminal_header_printf(const char *format, ...) {
	char intHolder[256];	// This is extremely bad practice but will do for now
	int index = 0;
	va_list parameters;
		va_start(parameters, format);

		int written = 0;
		size_t amount;
		bool rejected_bad_specifier = false;

		while (*format != '\0') {
			if (*format != '%') {
			print_c:
				amount = 1;
				while ( format[amount] && format[amount] != '%' )
					amount++;
				terminal_header_print(format, amount, index);
				index += amount;
				format += amount;
				written += amount;
				continue;
			}

			const char* format_begun_at = format;

			if (*(++format) == '%') {
				goto print_c;
			}

			if (rejected_bad_specifier) {
			incomprehensible_conversion:
				rejected_bad_specifier = true;
				format = format_begun_at;
				goto print_c;
			}

			if (*format == 'c') {
				format++;
				char c = (char) va_arg(parameters, int /* char promotes to int */);
				terminal_header_print(&c, sizeof(c), index);
				index += sizeof(c);
			} else if (*format == 's') {
				format++;
				const char* s = va_arg(parameters, const char*);
				terminal_header_print(s, strlen(s), index);
				index += strlen(s);
			} else if (*format == 'd' || *format == 'i') {
				format++;
				const char* d = itoa(va_arg(parameters, int), intHolder, 10);
				terminal_header_print(d, strlen(d), index);
				index += strlen(d);
			} else if (*format == 'x') {
				format++;
				const char* d = itoa(va_arg(parameters, int), intHolder, 16);
				terminal_header_print(d, strlen(d), index);
				index += strlen(d);
			} else if (isprint(*format)) {
				char padding = *format;
				int times = (char)(*(++format)) - '0';
				format++;

				if (*format == 'd' || *format == 'i') {
					format++;
					const char* d = itoa(va_arg(parameters, int), intHolder, 10);
					for (int i = 0; i < times - strlen(d); i++) {
						terminal_header_print(&padding, sizeof(padding), index);
						index += sizeof(padding);
					}
					terminal_header_print(d, strlen(d), index);
					index += strlen(d);
				} else if (*format == 'x') {
					format++;
					const char* d = itoa(va_arg(parameters, int), intHolder, 16);
					for (int i = 0; i < times - strlen(d); i++) {
						terminal_header_print(&padding, sizeof(padding), index);
						index += sizeof(padding);
					}
					terminal_header_print(d, strlen(d), index);
					index += strlen(d);
				} else {
					goto incomprehensible_conversion;
				}

			} else {
				goto incomprehensible_conversion;
			}
		}

		va_end(parameters);
}
 
void terminal_putchar(char c) {
	if (c == '\n') {
		terminal_row++;
		terminal_column = 0;
	} else if (c == '\t') {
		int increase = 4 - (terminal_column % 4);
		terminal_column += increase;
		if (terminal_column >= VGA_WIDTH) {
			terminal_row++;
			terminal_column = 0;
		}
	} else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if ( ++terminal_column == VGA_WIDTH ) {
			terminal_column = 0;
			if ( ++terminal_row == VGA_HEIGHT ) {
				terminal_scroll();
			}
		}
	}

	if (terminal_row >= VGA_HEIGHT) {
		terminal_scroll();
	}

	terminal_setcursor();
}
 
void terminal_write(const char* data, size_t size) {
	for ( size_t i = 0; i < size; i++ )
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
