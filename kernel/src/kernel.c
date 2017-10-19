#include <stdint.h>

void main(uint32_t magic, uint32_t addr) {
	const char* str = "PacketNgin ver 4.0";
	char* video = (char*)0xb8000;
	int i = 0;
	int j = 0;

	while(str[j] != 0) {
			video[i] = str[j];
			video[i + 1] = 0x07;
			j++;
			i+= 2;
	}

	asm volatile("movl %0, %%edx" : : "r"(magic));
	while(1) {
		/*
		if(magic == 0x36d76289)
			asm volatile("movl %0, %%edx" : : "r"(7));
		else
			asm volatile("movl %0, %%edx" : : "r"(8));
		*/
		asm volatile("hlt");
	}

	while(1)
		asm("hlt");

	return;
}
