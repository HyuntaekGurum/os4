; entry.asm
global start
global gdtr
global lgdt
global sgdt

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

	jmp 0x08:0x200000

lgdt:
	mov		eax, gdtr
	lgdt	[eax]
	jmp		0x18:.next
.next:
	mov		ax, 0x20
	mov		ds, ax
	mov		es, ax
	mov		fs, ax
	mov		gs, ax
	mov		ss, ax
	ret

sgdt:
	sgdt	[eax]
	ret

section .rodata
align 8, db 0
	dw	0x0000				; padding for GDTR

gdtr:
	dw	gdtend - gdt - 1	; GDT size
	dd	gdt 				; GDT address

gdt:
	; null descriptor
	dw	0x0000
	dw	0x0000
	db	0x00
	db	0x00
	db	0x00
	db	0x00

	; kernel code segment descriptor
	dw	0xffff	; limit 15:0
	dw	0x0000	; base 15:0
	db	0x00	; base 23:16
	db	0x9a	; p=1, dpl=0, code segment, execute/read
	db	0xaf	; g=1, d=0, l=1, limit 19:16
	db	0x00	; base 31:24

	; kernel data segment descriptor
	dw	0xffff	; limit 15:0
	dw	0x0000	; base 15:0
	db	0x00	; base 23:16
	db	0x92	; p=1, dpl=0, data segment, read/write
	db	0xaf	; g=1, d=0, l=1, limit 19:16
	db	0x00	; base 31:24

	; user code segment descriptor
	dw	0xffff	; limit 15:0
	dw	0x0000	; base 15:0
	db	0x00	; base 23:16
	db	0x9a	; p=1, dpl=0, code segment, read/write
	db	0xcf	; g=1, d=1, l=0, limit 19:16
	db	0x00	; base 31:24

	; user data segment descriptor
	dw	0xffff	; limit 15:0
	dw	0x0000	; base 15:0
	db	0x00	; base 23:16
	db	0x92	; p=1, dpl=0, data segment, read/write
	db	0xcf	; g=1, d=1, l=0, limit 19:16
	db	0x00	; base 31:24
gdtend:

section .bss
	resb 8192	; 8KB
stack:

