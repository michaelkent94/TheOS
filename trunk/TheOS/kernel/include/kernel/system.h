#ifndef _SYSTEM_H
#define _SYSTEM_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/interrupts.h>
#include <kernel/tables.h>
#include <kernel/timer.h>
#include <kernel/keyboard.h>
#include <kernel/rtc.h>

#include <asm/io.h>

#define _OS_NAME "TheOS"
#define _OS_VERSION "Alpha 1"

#define _OS_HEADER _OS_NAME " " _OS_VERSION "                                            %02d:%02d:%02d %s  %02d/%02d/%d"

#define STATUS_NORMAL make_color(COLOR_LIGHT_GREY, COLOR_BLACK)
#define STATUS_OK make_color(COLOR_LIGHT_BLUE, COLOR_BLACK)
#define STATUS_WARN make_color(COLOR_LIGHT_YELLOW, COLOR_BLACK)
#define STATUS_ERROR make_color(COLOR_LIGHT_RED, COLOR_BLACK)

#define cprintf(color, ...) terminal_setcolor(color); \
									printf(__VA_ARGS__); \
									terminal_setcolor(STATUS_NORMAL)
#define LOG(color, ...) cprintf(color, __VA_ARGS__)
#define WARN(...) cprintf(STATUS_WARN, __VA_ARGS__)
#define PANIC(...) cprintf(STATUS_ERROR, __VA_ARGS__)

#define DONE_OK() LOG(STATUS_OK, "\t\t\t\t\t\t\t\t\t\t\t\t[done]\n")
#define DONE_ERROR() PANIC("\t\t\t\t\t\t\t\t\t\t\t\t[error]\n")

#endif
