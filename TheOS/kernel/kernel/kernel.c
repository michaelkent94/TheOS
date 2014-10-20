#include <kernel/system.h>
#include <kernel/multiboot.h>

extern void _SETUP_START;
extern void _SETUP_END;
extern void _KERNEL_START;
extern void _KERNEL_END;

void printMessage() {
	printf("\nWelcome to " _OS_NAME " " _OS_VERSION "\n");
	for (int i = 0; i < 80; i++) {
		printf("-");
	}
}

void kinit() {
	terminal_initialize();
}

void updateTime(uint32_t tick) {
	rtc_update();
	terminal_header_printf(_OS_HEADER, rtc_getHours(), rtc_getMinutes(), rtc_getSeconds(), rtc_getPm() ? "PM" : "AM",
			rtc_getMonth(), rtc_getDay(), rtc_getYear());
}

void kmain(multiboot_info_t *mbd, uint32_t magic) {

	// memory
	phys_init(mbd, &_KERNEL_START, &_KERNEL_END);
	paging_init();

	// tables
	gdt_install();
	idt_install();

	timer_init();

	keyboard_install();

	// register a callback from the timer
	// to update the clock every second
	timer_callback_t updateTimeCallback;
	updateTimeCallback.callback = &updateTime;
	updateTimeCallback.period = 10;		// 10 times per second to be safe, the timer isn't perfectly accurate
	timer_registerCallback(&updateTimeCallback);

	// this marks the end of booting
	printMessage();

	// use this loop to do things continuously
	for (;;) {
		// refill the physical memory stack if it's empty for faster
		// allocation times on interrupts
		if (phys_is_stack_empty()) {
			phys_load_stack();
		}
	}
}
