#include "pit.h"

#define PIT_CONTROL     0x43
#define PIT_COUNTER0    0x40
#define PIT_FREQUENCY	1193182

static uint8_t port_in8(uint16_t port) {
	uint8_t ret;
	asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
	return ret;
}

static void port_out8(uint16_t port, uint8_t data) {
	asm volatile ("outb %%al,%%dx": :"d" (port), "a" (data));
}

static uint16_t read_counter() {
	port_out8(PIT_CONTROL, 0x00 << 6 | 0x00 << 4 | 0x01 << 1 | 0x00 << 0);  // SC=Counter0, RW=Latch, Mode=1, BCD=Binary
	return (uint16_t)port_in8(PIT_COUNTER0) | ((uint16_t)port_in8(PIT_COUNTER0) << 8);
}

static void wait(uint16_t freq) {
	port_out8(PIT_CONTROL, 0x00 << 6 | 0x03 << 4 | 0x01 << 1 | 0x00 << 0);	// SC=Counter0, RW=0b11, Mode=1, BCD=Binary

	port_out8(PIT_COUNTER0, freq & 0xff);
	port_out8(PIT_COUNTER0, freq >> 8);

	uint16_t count = freq;
	int i = 0;
	while(!(count == 0 || count > freq)) {
		count = read_counter();
	}
}

void pit_mwait(uint16_t ms) {
	uint32_t freq = PIT_FREQUENCY * ms / 1000;
	while(freq > 50000) {
		wait(50000);
		freq -= 50000;
	}
	wait(freq);
}

void pit_uwait(uint16_t us) {
	uint32_t freq = PIT_FREQUENCY * us / 1000000;
	while(freq > 50000) {
		wait(50000);
		freq -= 50000;
	}
	wait(freq);
}
