#include <stdio.h>
 
#if defined(__is_theos_kernel)
#include <kernel/screen/tty.h>
#endif
 
int putchar(int ic) {

#if defined(__is_theos_kernel)
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
#else
	// TODO: You need to implement a write system call.
#endif
	return ic;
}
