global start
extern main

bits 64
section .text
start:
	; Data segment descriptors
	mov	ax, 0x10
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	
	; Stack segment descriptors
	mov	ss, ax

	; Run kernel
	call	main
	
.loop:
	hlt
	hlt
	jmp	.loop
