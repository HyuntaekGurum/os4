; entry.asm
global start
extern main

bits 32
section .text
	; multiboot2
	align 8, db 0
multiboot_head:
	dd 0xe85250d6				; magic
	dd 0x00						; i386 protected mode
	dd multiboot_tail - multiboot_head
	dd 0x100000000 - (0xe85250d6 + 0x00 + (multiboot_tail - multiboot_head))	; checksum (magic + flags + checksum == 0)

	; Information request
	.info_start:
	dw  1
	dw  0
	dd  .info_end - .info_start
	dd  3               ; module
	dd  4               ; basic meminfo
	dd  5               ; bootdev
	dd  6               ; mmap
	.info_end:

	align 8, db 0
	; end tag
	.end_start:
	dw 0						; type
	dw 0						; flags
	dd .end_end - .end_start	; size
	.end_end:
multiboot_tail:

start:
	cli
	mov	esp, stack
	push ebx
	push eax
	call main
	hlt

section .bss
	resb 8192	; 8KB
stack:

