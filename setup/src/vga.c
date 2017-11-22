#include <stdint.h>
#include "vga.h"

#define HEX(v)  (((v) & 0x0f) > 9 ? ((v) & 0x0f) - 10 + 'a' : ((v) & 0x0f) + '0')

static char* vga_cursor;
static int vga_attr = VGA_NORMAL;

void vga_init() {
	vga_cursor = (char*)0xb8000 + 2 * vga_get_cursor();
}

uint16_t vga_get_cursor() {
	uint8_t idx1, idx2;

	asm volatile("outb %%al, %%dx" : : "d"(0x3d4), "a"(0x0f));
	asm volatile("inb %%dx, %%al" : "=a"(idx1) : "d"(0x3d5));
	asm volatile("outb %%al, %%dx" : : "d"(0x3d4), "a"(0x0e));
	asm volatile("inb %%dx, %%al" : "=a"(idx2) : "d"(0x3d5));

	return ((uint16_t)idx2 << 8 | (uint16_t)idx1);
}

void vga_set_cursor(uint16_t idx) {
	asm volatile("outb %%al, %%dx" : : "d"(0x3d4), "a"(0x0f));
	asm volatile("outb %%al, %%dx" : : "d"(0x3d5), "a"(idx & 0xff));
	asm volatile("outb %%al, %%dx" : : "d"(0x3d4), "a"(0x0e));
	asm volatile("outb %%al, %%dx" : : "d"(0x3d5), "a"((idx >> 8) & 0xff));
}

void vga_set_attr(uint8_t attr) {
	vga_attr = attr;
}

void vga_print_string(const char* str) {
	while(*str != 0) {
		if(*str == '\n') {
			vga_cursor = (char*)(((uint32_t)((uint32_t)vga_cursor - 0xb8000 + 160) / 160) * 160 + 0xb8000);
		} else {
			*vga_cursor++ = *str;
			*vga_cursor++ = vga_attr;
		}
		str++;
	}

	vga_set_cursor((uint16_t)(((uint32_t)vga_cursor - 0xb8000) / 2));
}

void vga_print_u8(uint8_t v) {
	*vga_cursor++ = HEX(v >> 4);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 0);
	*vga_cursor++ = vga_attr;

	vga_set_cursor((uint16_t)(((uint32_t)vga_cursor - 0xb8000) / 2));
}

void vga_print_u16(uint16_t v) {
	*vga_cursor++ = HEX(v >> 12);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 8);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 4);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 0);
	*vga_cursor++ = vga_attr;

	vga_set_cursor((uint16_t)(((uint32_t)vga_cursor - 0xb8000) / 2));
}

void vga_print_u32(uint32_t v) {
	*vga_cursor++ = HEX(v >> 28);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 24);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 20);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 16);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 12);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 8);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 4);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 0);
	*vga_cursor++ = vga_attr;

	vga_set_cursor((uint16_t)(((uint32_t)vga_cursor - 0xb8000) / 2));
}

void vga_print_u64(uint64_t v) {
	*vga_cursor++ = HEX(v >> 60);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 56);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 52);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 48);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 44);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 40);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 36);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 32);
	*vga_cursor++ = vga_attr;
	
	*vga_cursor++ = HEX(v >> 28);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 24);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 20);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 16);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 12);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 8);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 4);
	*vga_cursor++ = vga_attr;
	*vga_cursor++ = HEX(v >> 0);
	*vga_cursor++ = vga_attr;

	vga_set_cursor((uint16_t)(((uint32_t)vga_cursor - 0xb8000) / 2));
}

void vga_tab(uint32_t col) {
	uint32_t pos = (uint32_t)vga_cursor - 0xb8000;
	if(pos % 160 < col * 2) {
		vga_cursor += col * 2 - pos % 160;
	} else {
		vga_cursor += 160 - pos % 160 + col * 2;
	}

	vga_set_cursor((uint16_t)(((uint32_t)vga_cursor - 0xb8000) / 2));
}
