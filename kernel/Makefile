.PHONY: all clean

all: kernel.bin

obj/entry.o: src/entry.asm
	mkdir -p obj
	nasm -f elf64 src/entry.asm -o obj/entry.o

obj/%.o: src/%.c
	mkdir -p obj
	gcc -m64 -c $^ -o $@

kernel.elf: obj/entry.o obj/main.o
	ld -T link.ld -o $@ $^

kernel.bin: kernel.elf
	objcopy -O binary --only-section=.text kernel.elf kernel.bin

clean:
	rm -rf obj
	rm -f kernel.elf
	rm -f kernel.bin
