#include <stdint.h>
#include "multiboot2.h"
#include "vga.h"

static void pass() {
	vga_tab(75);
	vga_set_attr(0x02);
	vga_print_string("PASS\n");
	vga_set_attr(VGA_NORMAL);
}

static void fail() {
	vga_tab(75);
	vga_set_attr((0x04 | 0x08));
	vga_print_string("FAIL\n");
	vga_set_attr(VGA_NORMAL);

	while(1) {
		asm("hlt");
	}
}

static void check_multiboot(uint32_t magic, uint32_t addr) {
	vga_print_string("Checking multiboot2 boot loader magic: ");
	vga_print_u32(magic);
	if(magic == MULTIBOOT2_BOOTLOADER_MAGIC) {
		pass();
	} else {
		fail();
	}

	vga_print_string("Checking multiboot2 header: 0x");
	vga_print_u32(addr);
	if(addr != 0) {
		pass();
	} else {
		fail();
	}

	uint32_t end = addr + *(uint32_t*)addr;
	addr += 8;

	while(addr < end) {
		struct multiboot_tag* tag = (struct multiboot_tag*)addr;
		switch(tag->type) {
			case MULTIBOOT_TAG_TYPE_END:
				addr = end;
				break;
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				vga_tab(4);
				vga_print_string("Basic memory info\n");

				struct multiboot_tag_basic_meminfo* meminfo = (struct multiboot_tag_basic_meminfo*)tag;

				vga_tab(8);
				vga_print_string("Lower memory: ");
				vga_print_u32(meminfo->mem_lower);
				vga_print_string("\n");

				vga_tab(8);
				vga_print_string("Upper memory: ");
				vga_print_u32(meminfo->mem_upper);
				vga_print_string("\n");
				break;
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
				vga_tab(4);
				vga_print_string("Boot device\n");

				struct multiboot_tag_bootdev* bootdev = (struct multiboot_tag_bootdev*)tag;

				vga_tab(8);
				vga_print_string("BIOS dev: ");
				vga_print_u32(bootdev->biosdev);
				vga_print_string("\n");

				vga_tab(8);
				vga_print_string("Slice: ");
				vga_print_u32(bootdev->slice);
				vga_print_string("\n");

				vga_tab(8);
				vga_print_string("Part: ");
				vga_print_u32(bootdev->part);
				vga_print_string("\n");
				break;
			case MULTIBOOT_TAG_TYPE_MMAP:
				vga_tab(4);
				vga_print_string("Memory Map\n");

				struct multiboot_tag_mmap* mmap = (struct multiboot_tag_mmap*)tag;

				int count = (mmap->size - 8) / mmap->entry_size;
				for(int i = 0; i < count; i++) {
					vga_tab(8);
					vga_print_u64(mmap->entries[i].addr);
					vga_print_string(" - ");
					vga_print_u64(mmap->entries[i].addr + mmap->entries[i].len);
					vga_print_string(" : ");
					switch(mmap->entries[i].type) {
						case MULTIBOOT_MEMORY_AVAILABLE:
							vga_print_string("Available\n");
							break;
						case MULTIBOOT_MEMORY_RESERVED:
							vga_print_string("Reserved\n");
							break;
						case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
							vga_print_string("ACPI\n");
							break;
						case MULTIBOOT_MEMORY_NVS:
							vga_print_string("NVS\n");
							break;
						case MULTIBOOT_MEMORY_BADRAM:
							vga_print_string("BADRAM\n");
							break;
					}
				}

				break;
			case MULTIBOOT_TAG_TYPE_VBE:
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
			case MULTIBOOT_TAG_TYPE_APM:
			case MULTIBOOT_TAG_TYPE_EFI32:
			case MULTIBOOT_TAG_TYPE_EFI64:
			case MULTIBOOT_TAG_TYPE_SMBIOS:
			case MULTIBOOT_TAG_TYPE_ACPI_OLD:
			case MULTIBOOT_TAG_TYPE_ACPI_NEW:
			case MULTIBOOT_TAG_TYPE_NETWORK:
			case MULTIBOOT_TAG_TYPE_EFI_MMAP:
			case MULTIBOOT_TAG_TYPE_EFI_BS:
				break;
		}

		addr = (addr + tag->size + 7) & ~7;
	}
}

void main(uint32_t magic, uint32_t addr) {
	vga_init();
	vga_print_string("PacketNgin ver 4.0\n\n");

	check_multiboot(magic, addr);

	while(1)
		asm("hlt");

	return;
}
