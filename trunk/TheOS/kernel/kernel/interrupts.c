#include <kernel/interrupts.h>

isr_t handlers[256];

// This gets called from our ASM interrupt handler stub.
void isr_handler(const registers_t *regs) {

	if (handlers[regs->int_no] != 0) {
		isr_t handler = handlers[regs->int_no];
		handler(regs);
	} else {
		printf("Unhandled Interrupt 0x%x\n", regs->int_no);
		if (regs->err_code != 0) {
			printf("\terr_code = 0x%x\n", regs->err_code);
		}
		printf("\tds = 0x%x\n", regs->ds);
		printf("\tedi = 0x%x\n", regs->edi);
		printf("\tesi = 0x%x\n", regs->esi);
		printf("\tebp = 0x%x\n", regs->ebp);
		printf("\tebx = 0x%x\n", regs->ebx);
		printf("\tedx = 0x%x\n", regs->edx);
		printf("\tecx = 0x%x\n", regs->ecx);
		printf("\teax = 0x%x\n", regs->eax);
		printf("\teip = 0x%x\n", regs->eip);
		printf("\tcs = 0x%x\n", regs->cs);
		printf("\teflags = 0x%x\n", regs->eflags);
		printf("\tesp = 0x%x\n", regs->esp);
		printf("\tss = 0x%x\n", regs->ss);
	}
}

void irq_handler(const registers_t *regs) {
	// Send an EOI (end of interrupt) signal to the PICs.
	// If this interrupt involved the slave.
	if (regs->int_no >= 40) {
		// Send reset signal to slave.
		outb(0xa0, 0x20);
	}
	// Send reset signal to master. (As well as slave, if necessary).
	outb(0x20, 0x20);

	isr_handler(regs);
}

void idt_registerHandler(uint8_t index, isr_t handler) {
	handlers[index] = handler;
}
