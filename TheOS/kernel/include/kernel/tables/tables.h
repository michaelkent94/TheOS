#ifndef _TABLES_H
#define _TABLES_H 1

#include <stdint.h>

#include <kernel/tables/interrupts.h>

#include <asm/io.h>

typedef struct {

	uint16_t limit;
	uint32_t base;

} __attribute__((packed)) gdt_ptr_t;

typedef struct {

	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;

} __attribute__((packed)) gdt_entry_t;

typedef struct {

	uint16_t limit;
	uint32_t base;

} __attribute__((packed)) idt_ptr_t;

typedef struct {

	uint16_t base_low;
	uint16_t sel;
	uint8_t zero;
	uint8_t flags;
	uint16_t base_high;

} __attribute__((packed)) idt_entry_t;

void gdt_setEntry(int32_t, uint32_t, uint32_t, uint8_t, uint8_t);
void gdt_install();

void idt_setEntry(int32_t, uint32_t, uint16_t, uint8_t);
void idt_install();
void idt_registerHandler(uint8_t, isr_t);


#endif
