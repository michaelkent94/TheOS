#include <kernel/system.h>
#include <kernel/multiboot.h>

void printMessage() {
	printf("\nWelcome to " _OS_NAME " " _OS_VERSION "\n");
	for (int i = 0; i < 80; i++) {
		printf("-");
	}
}

void printMemoryInfo(multiboot_info_t *mbd) {
	printf("Memory info:\n");
	printf("\tLower Memory: 0x%x\n", mbd->mem_lower);
	printf("\tUpper Memory: 0x%x\n", mbd->mem_upper);
	printf("\tMap length: %d\n\tMap address: 0x%x\n", mbd->mmap_length, mbd->mmap_addr);
	int numEntries = mbd->mmap_length / sizeof(memory_map_t);
	printf("\tMap entries: %d\n", numEntries);

	printf("\tEntry\tType\tStart\t\t\t\t\tEnd\n");
	memory_map_t *entries = (memory_map_t *) mbd->mmap_addr;
	for (int i = 0; i < numEntries; i++) {
		memory_map_t entry = entries[i];
		uint64_t base_addr = (entry.base_addr_high << 32) | entry.base_addr_low;
		uint64_t length = (entry.length_high << 32) | entry.length_low;
		uint64_t end_addr = base_addr + length - 1;
		uint32_t end_addr_high = end_addr >> 32;
		uint32_t end_addr_low = end_addr & 0xffffffff;
		printf("\t%d\t\t%d\t\t0x%08x %08x\t\t0x%08x %08x\n", i, entry.type, entry.base_addr_high, entry.base_addr_low, end_addr_high, end_addr_low);
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

	paging_init();

	gdt_install();
	idt_install();

	timer_init();

	keyboard_install();

	printMessage();

	// register a callback from the timer
	// to update the clock every second
	timer_callback_t updateTimeCallback;
	updateTimeCallback.callback = &updateTime;
	updateTimeCallback.period = 10;		// 10 times per second to be safe, the timer isn't perfectly accurate
	timer_registerCallback(&updateTimeCallback);

	printMemoryInfo(mbd);

	for (;;);
}
