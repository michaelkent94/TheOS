OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS
{
	. = 0x100000;

	.setup :
	{
		_SETUP_START = .;
		*(.setup)
		_SETUP_END = .;
	}

	. += 0xC0000000;

	.text : AT(ADDR(.text) - 0xC0000000)
	{
		_KERNEL_START = .;
		*(.text)
	}

	.data ALIGN (4096) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
		*(.rodata*)
	}

	.bss ALIGN (4096) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON*)
		*(.bss*)
		_KERNEL_END = .;
	}
}
