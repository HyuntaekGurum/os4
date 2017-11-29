OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS {
	. = 0x10000;
	__kernel_start = .;
	.text : { 
		__text_start = .;
		*(.text) 
		__text_end = .;
	}
	.data : { 
		__data_start = .;
		*(.data) 
		__data_end = .;
	}
	.bss : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
	__kernel_end = .;
}
