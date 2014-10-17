[bits 32]
[section .text]
[global start]
[extern _init]
[extern kinit]
[extern kmain]

; Declare constants used for creating a multiboot header.
MULTIBOOT_ALIGN		equ 1<<0             						; align loaded modules on page boundaries
MULTIBOOT_MEMINFO 	equ 1<<1             						; provide memory map
MULTIBOOT_FLAGS 	equ MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO		; this is the Multiboot 'flag' field
MULTIBOOT_MAGIC		equ 0x1BADB002       						; 'magic number' lets bootloader find the header
MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS) 	; checksum of above, to prove we are multiboot

align 4
multiboot_header:
	dd MULTIBOOT_MAGIC
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM

; the kernel entry point
start:
	; here's the trick: we load a GDT with a base address
	; of 0x40000000 for the code (0x08) and data (0x10) segments
	lgdt [trickgdt]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
 
	; jump to the higher half kernel
	jmp 0x08:higherhalf
	
higherhalf:

    mov esp, sys_stack           ; set up the stack
    push eax                     ; pass Multiboot magic number

	; Initialize the core kernel before running the global constructors.
	; Containted in kernel.c
	call kinit

	; Call the global constructors.
	call _init

	push ebx	; push the multiboot info we get from grub to pass to the kernel

	; Transfer control to the main kernel.
	; Contained in kernel.c
	call kmain

	; Hang if kmain unexpectedly returns.
	cli
	hlt
	
[section .setup]

trickgdt:
	dw gdt_end - gdt - 1 ; size of the GDT
	dd gdt ; linear address of GDT
 
gdt:
	dd 0, 0							; null gate
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40	; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40	; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF
 
gdt_end:

[section .bss]

resb 0x1000
sys_stack:
