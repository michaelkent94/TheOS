#include <kernel/timer.h>
#include <kernel/system.h>

static uint32_t frequency;
static uint64_t tick = 0;

timer_callback_t *callbacks[64];

timer_callback_t blank_callback;

void callback(const registers_t *regs) {
	tick++;
	for (int i = 0; callbacks[i] != &blank_callback; i++) {
		timer_callback_t *c = callbacks[i];
		if ((tick - c->start) % (frequency / c->period) == 0) {
			timer_callback_function_t function = c->callback;
			function(tick);
		}
	}
}

void timer_registerCallback(timer_callback_t *callback) {
	static int nextCallbackIndex = 0;
	callback->start = tick;
	callbacks[nextCallbackIndex++] = callback;
}

void timer_init() {

	printf("Initializing timer...\n");

	idt_registerHandler(IRQ0, &callback);

	timer_setFrequency(10000);

	// setup the blank callbacks
	blank_callback.callback = (void *)0;
	blank_callback.period = 0;
	blank_callback.start = 0;
	for (int i = 0; i < 64; i++) {
		callbacks[i] = &blank_callback;	// clear all of the callbacks
	}

	DONE_OK();

}

void timer_setFrequency(uint32_t f) {
	frequency = f;

	// The value we send to the PIT is the value to divide it's input clock
	// (1193180 Hz) by, to get our required frequency. Important to note is
	// that the divisor must be small enough to fit into 16-bits.
	uint16_t divisor = 1193180 / frequency;

	// Send the command byte.
	outb(0x43, 0x36);

	// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
	uint8_t l = (uint8_t) (divisor & 0xFF);
	uint8_t h = (uint8_t) ((divisor >> 8) & 0xFF);

	// Send the frequency divisor.
	outb(0x40, l);
	outb(0x40, h);
}

uint64_t timer_getTick() {
	return tick;
}
