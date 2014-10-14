#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <sys/cdefs.h>
 
__BEGIN_DECLS
 
void terminal_initialize(void);
void terminal_clear();
void terminal_scroll();
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color);

void terminal_header_printf(const char *format, ...);

__END_DECLS
 
#endif
