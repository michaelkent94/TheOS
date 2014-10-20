#ifndef _TIMER_H
#define _TIMER_H 1

#include <stdint.h>

#include <kernel/tables/interrupts.h>

#include <asm/io.h>

typedef void (*timer_callback_function_t)(const uint32_t tick);

typedef struct {

	timer_callback_function_t *callback;
	uint32_t period;
	uint64_t start;

} timer_callback_t;

void timer_init();
uint64_t timer_getTick();
void timer_registerCallback(timer_callback_t *callback);

#endif
