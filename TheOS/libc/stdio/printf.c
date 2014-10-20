#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined(__is_theos_kernel)
#include <kernel/screen/tty.h>
#endif

char intHolder[256];	// This is extremely bad practice but will do for now

char* itoa(int32_t value, char *str, int base) {
	char *rc;
	char *ptr;
	char *low;
	// Check for supported base.
	if (base < 2 || base > 36) {
		*str = '\0';
		return str;
	}
	rc = ptr = str;
	// Set '-' for negative decimals.
	if (value < 0 && base == 10) {
		*ptr++ = '-';
	}
	// Remember where the numbers start.
	low = ptr;
	// The actual conversion.
	do {
		// Modulo is negative for negative value. This trick makes abs() unnecessary.
		*ptr++ =
				"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35
						+ value % base];
		value /= base;
	} while (value);
	// Terminating the string.
	*ptr-- = '\0';
	// Invert the numbers.
	while (low < ptr) {
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}
	return rc;
}

char* uitoa(uint32_t value, char *str, int base) {
	char *rc;
	char *ptr;
	char *low;
	// Check for supported base.
	if (base < 2 || base > 36) {
		*str = '\0';
		return str;
	}
	rc = ptr = str;

	// Remember where the numbers start.
	low = ptr;
	// The actual conversion.
	do {
		// Modulo is negative for negative value. This trick makes abs() unnecessary.
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
		value /= base;
	} while (value);
	// Terminating the string.
	*ptr-- = '\0';
	// Invert the numbers.
	while (low < ptr) {
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}
	return rc;
}

static void print(const char* data, size_t data_length) {
	for (size_t i = 0; i < data_length; i++) {
		putchar((int) ((const unsigned char*) data)[i]);
	}
}

int printf(const char* format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	size_t amount;
	bool rejected_bad_specifier = false;

	while (*format != '\0') {
		if (*format != '%') {
		print_c: amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format;

		if (*(++format) == '%') {
			goto print_c;
		}

		if (rejected_bad_specifier) {
			incomprehensible_conversion: rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		}

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			print(&c, sizeof(c));
		} else if (*format == 's') {
			format++;
			const char* s = va_arg(parameters, const char*);
			print(s, strlen(s));
		} else if (*format == 'd' || *format == 'i') {
			format++;
			const char* d = itoa(va_arg(parameters, int), intHolder, 10);
			print(d, strlen(d));
		} else if (*format == 'u') {
			format++;
			const char* u = uitoa(va_arg(parameters, int), intHolder, 10);
			print(u, strlen(u));
		} else if (*format == 'x') {
			format++;
			const char* x = uitoa(va_arg(parameters, int), intHolder, 16);
			print(x, strlen(x));
		} else if (isprint(*format)) {
			char padding = *format;
			int times;

			if (isdigit(*(++format))) {
				times = (char)(*format) - '0';
				format++;
			} else {
				times = padding - '0';
				padding = ' ';
			}

			if (*format == 'c') {
				format++;
				char c = (char) va_arg(parameters, int /* char promotes to int */);
				for (int i = 0; i < times - 1; i++) {
					print(&padding, sizeof(padding));
				}
				print(&c, sizeof(c));
			} else if (*format == 's') {
				format++;
				const char* s = va_arg(parameters, const char*);
				for (int i = 0; i < times - strlen(s); i++) {
					print(&padding, sizeof(padding));
				}
				print(s, strlen(s));
			} else if (*format == 'd' || *format == 'i') {
				format++;
				const char* d = itoa(va_arg(parameters, int), intHolder, 10);
				for (int i = 0; i < times - strlen(d); i++) {
					print(&padding, sizeof(padding));
				}
				print(d, strlen(d));
			} else if (*format == 'u') {
				format++;
				const char* u = uitoa(va_arg(parameters, int), intHolder, 10);
				for (int i = 0; i < times - strlen(u); i++) {
					print(&padding, sizeof(padding));
				}
				print(u, strlen(u));
			} else if (*format == 'x') {
				format++;
				const char* x = uitoa(va_arg(parameters, int), intHolder, 16);
				for (int i = 0; i < times - strlen(x); i++) {
					print(&padding, sizeof(padding));
				}
				print(x, strlen(x));
			} else {
				goto incomprehensible_conversion;

			}

		} else {
			goto incomprehensible_conversion;

		}
	}

	va_end(parameters);

	return written;
}
