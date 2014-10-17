.global _loader

# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# This is the virtual base address of kernel space. It must be used to convert virtual
# addresses into physical addresses until paging is enabled. Note that this is not
# the virtual address where the kernel image itself is loaded -- just the amount that must
# be subtracted from a virtual address to get a physical address.
.set KERNEL_VIRTUAL_BASE, 0xC0000000		# 3GB
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)		# Page directory index of kernel's 4MB PTE

.section .data
.align 0x1000
BootPageDirectory:
	# This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
    # All bits are clear except the following:
    # bit 7: PS The kernel page is 4MB.
    # bit 1: RW The kernel page is read/write.
    # bit 0: P  The kernel page is present.
    # This entry must be here -- otherwise the kernel will crash immediately after paging is
    # enabled because it can't fetch the next instruction! It's ok to unmap this page later.
    .long 0x00000083
    .fill (KERNEL_PAGE_NUMBER - 1), 4, 0
    # This page directory entry defines a 4MB page containing the kernel.
    .long 0x00000083
    .fill (1024 - KERNEL_PAGE_NUMBER - 1), 4, 0  # Pages after the kernel image.

.section .text
# Declare a header as in the Multiboot Standard.
.align 4
MultibootHeader:
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

# The kernel entry point.
.set loader, (_loader - 0xC0000000)
.global loader

_loader:

	# NOTE: Until paging is set up, the code must be position-independent and use physical
    # addresses, not virtual ones!
    mov $(BootPageDirectory - KERNEL_VIRTUAL_BASE), %ecx
    mov %ecx, %cr3                                       # Load Page Directory Base Register.

    mov %cr4, %ecx
    or $0x00000010, %ecx                        # Set PSE bit in CR4 to enable 4MB pages.
    mov %ecx, %cr4

    mov %cr0, %ecx
    or $0x80000000, %ecx                          # Set PG bit in CR0 to enable paging.
    mov %ecx, %cr0

    # Start fetching instructions in kernel space.
    # Since eip at this point holds the physical address of this command (approximately 0x00100000)
    # we need to do a long jump to the correct virtual address of StartInHigherHalf which is
    # approximately 0xC0100000.
    lea _start, %ecx
    jmp %ecx

_start:
	# Unmap the identity-mapped first 4MB of physical address space. It should not be needed
    # anymore.
    movl $0, (BootPageDirectory)
    invlpg (0)

	movl $stack_top, %esp

	# Initialize the core kernel before running the global constructors.
	# Containted in kernel.c
	call kinit

	# Call the global constructors.
	call _init

	push %ebx	# push the multiboot info we get from grub to pass to the kernel

	# Transfer control to the main kernel.
	# Contained in kernel.c
	call kmain

	# Hang if kmain unexpectedly returns.
	cli
	hlt
.Lhang:
	jmp .Lhang

.section .bss
.align 32
stack_bottom:
.skip 0x4000 # 16 KB
stack_top:
