OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(start)
SECTIONS {
	. = 0x200000;
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
