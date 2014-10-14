#include <kernel/tables.h>
#include <kernel/system.h>

/**************************************************
 * GDT
 **************************************************/

extern void gdt_flush();

gdt_entry_t gdtEntries[5];
gdt_ptr_t gdtp;

void gdt_setEntry(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
	gdtEntries[index].base_low = (base & 0xFFFF);
	gdtEntries[index].base_middle = (base >> 16) & 0xFF;
	gdtEntries[index].base_high = (base >> 24) & 0xFF;

	gdtEntries[index].limit_low = (limit & 0xFFFF);
	gdtEntries[index].granularity = (limit >> 16) & 0x0F;

	gdtEntries[index].granularity |= gran & 0xF0;
	gdtEntries[index].access = access;
}

void gdt_install() {

	printf("Installing GDT...\n");

	gdtp.limit = sizeof(gdtEntries) - 1;
	gdtp.base = (uint32_t) &gdtEntries;

	gdt_setEntry(0, 0, 0, 0, 0);					// null descriptor
	gdt_setEntry(1, 0, 0xffffffff, 0x9a, 0xcf);		// code segment
	gdt_setEntry(2, 0, 0xffffffff, 0x92, 0xcf);		// data segment
	gdt_setEntry(3, 0, 0xffffffff, 0xfa, 0xcf);		// user space code segment
	gdt_setEntry(4, 0, 0xffffffff, 0xf2, 0xcf);		// user space data segment

	gdt_flush();

	DONE_OK();
}

/**************************************************
 * IDT
 **************************************************/

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

extern void idt_flush();

extern isr_t handlers;

idt_entry_t idtEntries[256];
idt_ptr_t idtp;

void idt_setEntry(int32_t index, uint32_t base, uint16_t sel, uint8_t flags) {

	idtEntries[index].base_low = base & 0xffff;
	idtEntries[index].base_high = (base >> 16) & 0xffff;

	idtEntries[index].sel = sel;
	idtEntries[index].zero = 0;

	// TODO when going to user mode, uncomment the OR
	// it sets the interrupt privilege level to 3
	idtEntries[index].flags = flags/* | 0x60*/;

}

void idt_install() {

	printf("Installing IDT...\n");

	idtp.limit = sizeof(idtEntries) - 1;
	idtp.base = (uint32_t) &idtEntries;

	memset(&idtEntries, 0, sizeof(idtEntries));
	memset(&handlers, 0, sizeof(handlers));

	printf("\tInstalling ISRs...\n");

	idt_setEntry(0, (uint32_t) &isr0, 0x08, 0x8e);
	idt_setEntry(1, (uint32_t) &isr1, 0x08, 0x8e);
	idt_setEntry(2, (uint32_t) &isr2, 0x08, 0x8e);
	idt_setEntry(3, (uint32_t) &isr3, 0x08, 0x8e);
	idt_setEntry(4, (uint32_t) &isr4, 0x08, 0x8e);
	idt_setEntry(5, (uint32_t) &isr5, 0x08, 0x8e);
	idt_setEntry(6, (uint32_t) &isr6, 0x08, 0x8e);
	idt_setEntry(7, (uint32_t) &isr7, 0x08, 0x8e);
	idt_setEntry(8, (uint32_t) &isr8, 0x08, 0x8e);
	idt_setEntry(9, (uint32_t) &isr9, 0x08, 0x8e);
	idt_setEntry(10, (uint32_t) &isr10, 0x08, 0x8e);
	idt_setEntry(11, (uint32_t) &isr11, 0x08, 0x8e);
	idt_setEntry(12, (uint32_t) &isr12, 0x08, 0x8e);
	idt_setEntry(13, (uint32_t) &isr13, 0x08, 0x8e);
	idt_setEntry(14, (uint32_t) &isr14, 0x08, 0x8e);
	idt_setEntry(15, (uint32_t) &isr15, 0x08, 0x8e);
	idt_setEntry(16, (uint32_t) &isr16, 0x08, 0x8e);
	idt_setEntry(17, (uint32_t) &isr17, 0x08, 0x8e);
	idt_setEntry(18, (uint32_t) &isr18, 0x08, 0x8e);
	idt_setEntry(19, (uint32_t) &isr19, 0x08, 0x8e);
	idt_setEntry(20, (uint32_t) &isr20, 0x08, 0x8e);
	idt_setEntry(21, (uint32_t) &isr21, 0x08, 0x8e);
	idt_setEntry(22, (uint32_t) &isr22, 0x08, 0x8e);
	idt_setEntry(23, (uint32_t) &isr23, 0x08, 0x8e);
	idt_setEntry(24, (uint32_t) &isr24, 0x08, 0x8e);
	idt_setEntry(25, (uint32_t) &isr25, 0x08, 0x8e);
	idt_setEntry(26, (uint32_t) &isr26, 0x08, 0x8e);
	idt_setEntry(27, (uint32_t) &isr27, 0x08, 0x8e);
	idt_setEntry(28, (uint32_t) &isr28, 0x08, 0x8e);
	idt_setEntry(29, (uint32_t) &isr29, 0x08, 0x8e);
	idt_setEntry(30, (uint32_t) &isr30, 0x08, 0x8e);
	idt_setEntry(31, (uint32_t) &isr31, 0x08, 0x8e);

	printf("\tRemapping IRQs...\n");

	outb(0x20, 0x11);
	outb(0xa0, 0x11);
	outb(0x21, 0x20);
	outb(0xa1, 0x28);
	outb(0x21, 0x04);
	outb(0xa1, 0x02);
	outb(0x21, 0x01);
	outb(0xa1, 0x01);
	outb(0x21, 0x0);
	outb(0xa1, 0x0);

	printf("\tInstalling IRQs...\n");

	idt_setEntry(32, (uint32_t) &irq0, 0x08, 0x8e);
	idt_setEntry(33, (uint32_t) &irq1, 0x08, 0x8e);
	idt_setEntry(34, (uint32_t) &irq2, 0x08, 0x8e);
	idt_setEntry(35, (uint32_t) &irq3, 0x08, 0x8e);
	idt_setEntry(36, (uint32_t) &irq4, 0x08, 0x8e);
	idt_setEntry(37, (uint32_t) &irq5, 0x08, 0x8e);
	idt_setEntry(38, (uint32_t) &irq6, 0x08, 0x8e);
	idt_setEntry(39, (uint32_t) &irq7, 0x08, 0x8e);
	idt_setEntry(40, (uint32_t) &irq8, 0x08, 0x8e);
	idt_setEntry(41, (uint32_t) &irq9, 0x08, 0x8e);
	idt_setEntry(42, (uint32_t) &irq10, 0x08, 0x8e);
	idt_setEntry(43, (uint32_t) &irq11, 0x08, 0x8e);
	idt_setEntry(44, (uint32_t) &irq12, 0x08, 0x8e);
	idt_setEntry(45, (uint32_t) &irq13, 0x08, 0x8e);
	idt_setEntry(46, (uint32_t) &irq14, 0x08, 0x8e);
	idt_setEntry(47, (uint32_t) &irq15, 0x08, 0x8e);


	idt_flush();

	DONE_OK();
}
