#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/system.h>
 
__attribute__((__noreturn__))
void abort(void) {
	// TODO: panic better than this
	PANIC("Kernel Panic: abort()\n");
	asm volatile ("cli");
	while (1) { }
	__builtin_unreachable();
}
