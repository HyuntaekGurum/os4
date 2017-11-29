#include <stdint.h>

void __stack_chk_fail() {
	while(1) {
		asm("hlt");
	}
}

static void cpuid(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
	asm volatile("cpuid"
		: "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
		: "a"(*a), "c"(*c));
}

static uint8_t get_apic_id() {
	uint32_t a = 0x01, b, c, d;
	cpuid(&a, &b, &c, &d);

	return (b >> 24) & 0xff;
}

void main() {
	uint8_t apic_id = get_apic_id();

	char* ch = (char*)0xb8000 + apic_id * 160;
	*ch++ = 'H';
	*ch++ = 0x07;
	*ch++ = 'e';
	*ch++ = 0x07;
	*ch++ = 'l';
	*ch++ = 0x07;
	*ch++ = 'l';
	*ch++ = 0x07;
	*ch++ = 'o';
	*ch++ = 0x07;
	*ch++ = ' ';
	*ch++ = 0x07;
	*ch++ = '6';
	*ch++ = 0x07;
	*ch++ = '4';
	*ch++ = 0x07;
	*ch++ = 'b';
	*ch++ = 0x07;
	*ch++ = 'i';
	*ch++ = 0x07;
	*ch++ = 't';
	*ch++ = 0x07;
	*ch++ = ' ';
	*ch++ = 0x07;
	*ch++ = 's';
	*ch++ = 0x07;
	*ch++ = 'i';
	*ch++ = 0x07;
	*ch++ = 'z';
	*ch++ = 0x07;
	*ch++ = 'e';
	*ch++ = 0x07;
	*ch++ = 'o';
	*ch++ = 0x07;
	*ch++ = 'f';
	*ch++ = 0x07;
	*ch++ = '(';
	*ch++ = 0x07;
	*ch++ = 'l';
	*ch++ = 0x07;
	*ch++ = 'o';
	*ch++ = 0x07;
	*ch++ = 'n';
	*ch++ = 0x07;
	*ch++ = 'g';
	*ch++ = 0x07;
	*ch++ = ')';
	*ch++ = 0x07;
	*ch++ = '=';
	*ch++ = 0x07;
	*ch++ = '0' + sizeof(long);
	*ch++ = 0x07;
	*ch++ = ':';
	*ch++ = 0x07;
	*ch++ = '0' + apic_id;
	*ch++ = 0x07;
}
