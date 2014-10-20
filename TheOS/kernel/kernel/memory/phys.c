#include <kernel/memory/phys.h>
#include <kernel/system.h>

#define FRAMES 0x100000
#define FRAMES_BYTES (FRAMES >> 3)
#define STACK_SIZE 1024

uint8_t bitmap[FRAMES_BYTES];	// this is enough entries for 1bit/frame
uint32_t stack[STACK_SIZE];		// this is enough to hold pointers to 1024 free frames
int stack_ptr = STACK_SIZE - 1;

static void bitmap_setBit(int i) {
	bitmap[i / 8] |= 1 << (i % 8);
}

static void bitmap_clearBit(int i) {
	bitmap[i / 8] &= ~(1 << (i % 8));
}

static int bitmap_getBit(int i) {
	return (bitmap[i / 8] >> (i % 8)) & 1;
}

static void stack_push(phys_addr_t addr) {
	stack[stack_ptr--] = addr;
}

static phys_addr_t stack_pop() {
	return stack[++stack_ptr];
}

static phys_addr_t physFromBit(int i) {
	return i << 12;
}

void phys_init(multiboot_info_t *mbd, phys_addr_t kstart, phys_addr_t kend) {
	// assume all memory is free at first
	for (int i = 0; i < FRAMES_BYTES; i++) {
		bitmap[i] = 0;
	}

	// clear the stack
	for (int i = 0; i < STACK_SIZE; i++) {
		stack[i] = 0;
	}

	// now find out what is not free from the multiboot info
	int numEntries = mbd->mmap_length / sizeof(memory_map_t);
	memory_map_t *entries = (memory_map_t *) mbd->mmap_addr;
	for (int i = 0; i < numEntries; i++) {
		memory_map_t entry = entries[i];

		// if it's not free
		if (entry.type != 1) {
			uint64_t base_addr = (entry.base_addr_high << 32) | entry.base_addr_low;
			uint64_t length = (entry.length_high << 32) | entry.length_low;
			uint64_t end_addr = base_addr + length - 1;

			// find the start and end indices
			uint32_t firstFrame = base_addr >> 12;
			uint32_t lastFrame = (end_addr & 0xfff) ? (end_addr >> 12) + 1 : end_addr >> 12;		// we might need to go one past

			for (int bit = firstFrame; bit <= lastFrame; bit++) {
				bitmap_setBit(bit);
			}
		}
	}

	// make sure to mark the kernel frames as used
	kstart &= ~0xfff;
	kend = (kend & ~0xfff) + 1;
	for (int i = kstart; i <= kend; i++) {
		bitmap_setBit(i);
	}

	// now setup the stack initially
	phys_load_stack();
}

void phys_load_stack() {
	for (int i = FRAMES - 1; i >= 0 && stack_ptr >= 0; i--) {
		if (!bitmap_getBit(i)) {
			stack_push(physFromBit(i));
		}
	}
}

bool phys_is_stack_empty() {
	return stack_ptr == STACK_SIZE - 1;
}

bool phys_is_stack_full() {
	return stack_ptr == 0;
}

phys_addr_t phys_frame_alloc() {
	if (phys_is_stack_empty()) {
		phys_load_stack();
	}

	// if it's still empty... we have a problem
	if (phys_is_stack_empty()) {
		PANIC("phys_frame_alloc(): No more free page frames\n");
	}

	phys_addr_t frame = stack_pop();
	bitmap_setBit(frame >> 12);
	return frame;
}

void phys_frame_free(phys_addr_t frame) {
	bitmap_clearBit(frame >> 12);

	if (!phys_is_stack_full()) {
		stack_push(frame);
	}
}
