.PHONY: all clean

all: kernel.bin

obj/entry.o: src/entry.asm
	mkdir -p obj
	nasm -f elf32 src/entry.asm -o obj/entry.o

obj/%.o: src/%.c
	mkdir -p obj
	gcc -m32 -c $^ -o $@

kernel.bin: obj/entry.o obj/kernel.o obj/vga.o
	ld -m elf_i386 -T link.ld -o kernel.bin obj/entry.o obj/kernel.o obj/vga.o

clean:
	rm -rf obj
	rm kernel.bin
