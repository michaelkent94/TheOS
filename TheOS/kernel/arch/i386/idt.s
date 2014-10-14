.macro ISR_NOERRCODE i
  	.global isr\i
  	isr\i:
    	cli
    	pushl $0
    	pushl $\i
    	jmp isr_common_stub
.endm

.macro ISR_ERRCODE i
	.global isr\i
	isr\i:
		cli
		pushl $\i
		jmp isr_common_stub
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

.global idt_flush
.extern idtp

idt_flush:
	lidt idtp
	sti
	ret


.extern isr_handler
# This is our common ISR stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.
isr_common_stub:
	pusha				# Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

	mov %ds, %ax		# Lower 16-bits of eax = ds.
	push %eax			# save the data segment descriptor

	mov $0x10, %ax		# load the kernel data segment descriptor
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	# we need to push a pointer to this registers struct now on the stack
	push %esp			# pass it into isr_handler

	call isr_handler

	pop %eax			# take care of the parameter we pushed

	pop %eax			# reload the original data segment descriptor
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa				# Pops edi,esi,ebp...
	add $8, %esp		# Cleans up the pushed error code and pushed ISR number
	iret				# pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
