.PHONY: all clean

all: setup.elf

obj/entry.o: src/entry.asm
	mkdir -p obj
	nasm -f elf32 src/entry.asm -o obj/entry.o

obj/%.o: src/%.c
	mkdir -p obj
	gcc -m32 -c $^ -o $@

setup.elf: obj/entry.o obj/main.o obj/vga.o obj/pit.o obj/apic.o
	ld -m elf_i386 -T link.ld -o $@ $^

clean:
	rm -rf obj
	rm -f setup.elf
