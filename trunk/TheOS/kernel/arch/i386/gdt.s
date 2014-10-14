.global gdt_flush
.extern gdtp

gdt_flush:
	lgdt gdtp
	mov $0x10, %ax      # 0x10 is the offset in the GDT to our data segment
	mov %ax, %ds        # Load all data segment selectors
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x08,$_flush   # 0x08 is the offset to our code segment: Far jump!

_flush:
   ret
