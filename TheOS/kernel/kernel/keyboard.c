#include <kernel/keyboard.h>
#include <kernel/system.h>
#include <stdbool.h>
#include <ctype.h>

static bool left_shift = false;
static bool right_shift = false;
static bool caps_lock = false;
static bool scroll_lock = false;
static bool num_lock = false;

static bool left_ctrl = false;
static bool right_ctrl = false;
static bool left_alt = false;
static bool right_alt = false;
static bool left_cmd = false;
static bool right_cmd = false;

uint8_t scanCodes[] = {ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
		'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
		LEFT_CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', LEFT_SHIFT, '\\',
		'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', RIGHT_SHIFT, '*'/* keypad */, LEFT_ALT, ' ',
		CAPS_LOCK, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
		NUM_LOCK, SCROLL_LOCK, '7'/* keypad */, '8'/* keypad */, '9'/* keypad */, '-'/* keypad */,
		'4'/* keypad */, '5'/* keypad */, '6'/* keypad */, '+'/* keypad */,
		'1'/* keypad */, '2'/* keypad */, '3'/* keypad */, '0'/* keypad */, '.'/* keypad */,
		0, 0, 0, F11, F12
};

uint8_t shift_scanCodes[] = {ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
		'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
		LEFT_CTRL, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', LEFT_SHIFT, '|',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', RIGHT_SHIFT, '*'/* keypad */, LEFT_ALT, ' ',
		CAPS_LOCK, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
		NUM_LOCK, SCROLL_LOCK, '7'/* keypad */, '8'/* keypad */, '9'/* keypad */, '-'/* keypad */,
		'4'/* keypad */, '5'/* keypad */, '6'/* keypad */, '+'/* keypad */,
		'1'/* keypad */, '2'/* keypad */, '3'/* keypad */, '0'/* keypad */, '.'/* keypad */,
		0, 0, 0, F11, F12
};

uint8_t extended_scanCodes[] = {'\n'/* keypad */, RIGHT_CTRL, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		'/'/* keypad */, 0, 0, RIGHT_ALT,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		HOME, UP, PAGE_UP, 0, LEFT, 0, RIGHT,
		0, END, DOWN, PAGE_DOWN, INSERT, DELETE,
		0, 0, 0, 0, 0, 0, 0,
		LEFT_CMD, RIGHT_CMD, APPS
};

bool key_isPrintable(uint8_t key) {
	return isprint(key);
}

void key_print(uint8_t key) {
	if (key_isPrintable(key)) {
		printf("%c", key);
	} else {
		LOG(STATUS_OK, " %d ", key);
	}
}

void key_pressed(uint8_t key) {
	switch (key) {
	case LEFT_SHIFT:
		left_shift = true;
		break;
	case RIGHT_SHIFT:
		right_shift = true;
		break;
	case LEFT_CTRL:
		left_ctrl = true;
		break;
	case RIGHT_CTRL:
		right_ctrl = true;
		break;
	case LEFT_ALT:
		left_alt = true;
		break;
	case RIGHT_ALT:
		right_alt = true;
		break;
	case LEFT_CMD:
		left_cmd = true;
		break;
	case RIGHT_CMD:
		right_cmd = true;
		break;

	case CAPS_LOCK:
		caps_lock = !caps_lock;
		keyboard_setStatus();
		break;
	case SCROLL_LOCK:
		scroll_lock = !scroll_lock;
		keyboard_setStatus();
		break;
	case NUM_LOCK:
		num_lock = !num_lock;
		keyboard_setStatus();
		break;
	default:
		key_print(key);
	}
}

void key_released(uint8_t key) {
	switch (key) {
	case LEFT_SHIFT:
		left_shift = false;
		break;
	case RIGHT_SHIFT:
		right_shift = false;
		break;
	case LEFT_CTRL:
		left_ctrl = false;
		break;
	case RIGHT_CTRL:
		right_ctrl = false;
		break;
	case LEFT_ALT:
		left_alt = false;
		break;
	case RIGHT_ALT:
		right_alt = false;
		break;
	case LEFT_CMD:
		left_cmd = false;
		break;
	case RIGHT_CMD:
		right_cmd = false;
		break;
	}
}

void keyboard_setStatus() {

	uint8_t status = 0;

	status = scroll_lock ? status | 1 : status & 1;
	status = num_lock ? status | 2 : status & 2;
	status = caps_lock ? status | 4 : status & 4;

	outb(0x60, 0xed);
	outb(0x60, status);
}

void keyboard_handler(const registers_t *regs) {

	// stores whether the last received scan code was 0xe0
	// which means we'll be in the extended table
	static bool extended = false;

	uint8_t scanCode = inb(0x60);

	if (!extended) {

		if (scanCode < 0x59) {	// pressed

			if ((left_shift && !caps_lock) || (right_shift && !caps_lock) ||
					(!left_shift && !right_shift && caps_lock)) {
				key_pressed(shift_scanCodes[scanCode - 1]);
			} else {
				key_pressed(scanCodes[scanCode - 1]);
			}

		} else if (scanCode < 0xd9) {	// released

			scanCode -= 0x80;

			if ((left_shift && !caps_lock) || (right_shift && !caps_lock) ||
					(!left_shift && !right_shift && caps_lock)) {
				key_released(shift_scanCodes[scanCode - 1]);
			} else {
				key_released(scanCodes[scanCode - 1]);
			}

		} else if (scanCode == 0xe0) {
			extended = true;
		}

	} else {

		extended = false;

		if (scanCode < 0x5e) {	// pressed

			scanCode -= 0x1c;
			key_pressed(extended_scanCodes[scanCode]);

		} else if (scanCode < 0xde) {	// released

			scanCode -= 0x9c;
			key_released(extended_scanCodes[scanCode]);

		} else if (scanCode == 0xe0) {
			extended = true;
		}
	}
}

void keyboard_install() {
	printf("Installing Keyboard...\n");

	idt_registerHandler(33, keyboard_handler);

	DONE_OK();
}
