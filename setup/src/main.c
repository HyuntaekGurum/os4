#include <stdint.h>
#include "multiboot2.h"
#include "vga.h"
#include "page.h"

void __stack_chk_fail() {
	vga_set_attr((0x04 | 0x08));
	vga_print_string("Stack overflow\n");

	while(1) {
		asm("hlt");
	}
}

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
	vga_print_string("Check multiboot2 boot loader magic: ");
	vga_print_u32(magic);
	if(magic == MULTIBOOT2_BOOTLOADER_MAGIC) {
		pass();
	} else {
		fail();
	}

	vga_print_string("Check multiboot2 header: 0x");
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
				vga_tab(4);
				vga_print_string("Module 0x");

				struct multiboot_tag_module* module = (struct multiboot_tag_module*)tag;
				vga_print_u32(module->mod_start);
				vga_print_string("(0x");
				vga_print_u32(module->mod_end - module->mod_start);
				vga_print_string(") ");
				vga_print_string(module->cmdline);
				vga_print_string("\n");

				uint8_t* src = (void*)module->mod_start;
				uint8_t* end = (void*)module->mod_end;
				uint8_t* dst = (void*)0x200000;
				while(src <= end)
					*dst++ = *src++;

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
				/*
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
				*/

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

static void check_cpuid() {
	uint32_t flags = 0;
	asm volatile("pushfl\n"
		"pop %0\n" : "=r"(flags));

	vga_print_string("Check CPUID supported: ");
	vga_print_u32(flags);

	if((flags & (1 << 21)) != 0) {
		pass();
	} else {
		fail();
	}
}

static void cpuid(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
	asm volatile("cpuid"
		: "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
		: "a"(*a), "c"(*c));
}

static void check_longmode() {
	uint32_t a = 0x80000000, b, c = 0x00, d;
	cpuid(&a, &b, &c, &d);

	vga_print_string("Check CPUID extention supported: ");
	vga_print_u32(a);
	if(a > 0x80000000) {
		pass();
	} else {
		fail();
	}

	a = 0x80000001, c = 0x00;
	cpuid(&a, &b, &c, &d);
	vga_print_string("Check long-mode supported: ");
	vga_print_u32(d);

	if((d & (1 << 29)) != 0) {
		pass();
	} else {
		fail();
	}
}

static void load_gdt(uint8_t apic_id) {
	extern uint64_t gdtr;
	if(apic_id == 0) {
		vga_print_string("Load GDT register: 0x");
		vga_print_u32((gdtr >> 16) & 0xffffffff);
		vga_print_string(" (0x");
		vga_print_u16(gdtr & 0xffff);
		vga_print_string(")");
	}

	// Load GDT register
	void lgdt();
	lgdt();

	if(apic_id == 0) {
		pass();
	}
}

static void activate_pae(uint8_t apic_id) {
	#define PAE 0x620	// OSXMMEXCPT=1, OSFXSR=1, PAE=1

	uint32_t cr4;
	asm volatile("movl %%cr4, %0" : "=r"(cr4));

	cr4 |= PAE;
	asm volatile("movl %0, %%cr4" : : "r"(cr4));

	if(apic_id == 0) {
		vga_print_string("Activate physical address extension: 0x");
		vga_print_u32(cr4);
		pass();
	}
}

/**
 * TLB size: 256KB
 * TLB address: 6MB + apic_id * 2MB - 256KB
 * TLB Blocks(4K blocks)
 * TLB[0] => l2
 * TLB[1] => l3u
 * TLB[2] => l3k
 * TLB[3~61] => l4u
 * TLB[62~63] => l4k
 */
static void init_page_tables(uint8_t apic_id) {
	uint32_t base = 0x600000 + apic_id * 0x200000 - 0x40000;
	if(apic_id == 0) {
		vga_print_string("Initializing page table: 0x");
		vga_print_u32(base);
	}

	PageDirectory* l2 = (PageDirectory*)(base + PAGE_TABLE_SIZE * PAGE_L2_INDEX);
	PageDirectory* l3u = (PageDirectory*)(base + PAGE_TABLE_SIZE * PAGE_L3U_INDEX);
	PageDirectory* l3k = (PageDirectory*)(base + PAGE_TABLE_SIZE * PAGE_L3K_INDEX);
	PageTable* l4u = (PageTable*)(base + PAGE_TABLE_SIZE * PAGE_L4U_INDEX);
	PageTable* l4k = (PageTable*)(base + PAGE_TABLE_SIZE * PAGE_L4K_INDEX);

	// Clean Page Table area
	volatile uint32_t* p = (uint32_t*)base;
	for(int i = 0; i < 65536; i++)
		*p++ = 0;

	// Level 2
	l2[0].base = (uint32_t)l3u >> 12;
	l2[0].p = 1;
	l2[0].us = 1;
	l2[0].rw = 1;

	l2[511].base = (uint32_t)l3k >> 12;
	l2[511].p = 1;
	l2[511].us = 0;
	l2[511].rw = 1;

	// Level 3
	for(int i = 0; i < PAGE_L4U_SIZE; i++) {
		l3u[i].base = (uint32_t)&l4u[i * PAGE_ENTRY_COUNT] >> 12;
		l3u[i].p = 1;
		l3u[i].us = 1;
		l3u[i].rw = 1;
	}

	for(int i = 0; i < PAGE_L4K_SIZE; i++) {
		l3k[PAGE_ENTRY_COUNT - PAGE_L4K_SIZE + i].base = (uint32_t)&l4k[i * PAGE_ENTRY_COUNT] >> 12;
		l3k[PAGE_ENTRY_COUNT - PAGE_L4K_SIZE + i].p = 1;
		l3k[PAGE_ENTRY_COUNT - PAGE_L4K_SIZE + i].us = 0;
		l3k[PAGE_ENTRY_COUNT - PAGE_L4K_SIZE + i].rw = 1;
	}

	// Level 4
	for(int i = 0; i < PAGE_L4U_SIZE * PAGE_ENTRY_COUNT; i++) {
		l4u[i].base = i;
		l4u[i].p = 1;
		l4u[i].us = 0;
		l4u[i].rw = 1;
		l4u[i].ps = 1;
	}

	// Kernel global area(gmalloc, segment descriptor, IDT, code, rodata)
	// Mapping 256MB to kernel
	for(int i = 0; i < 128; i++) {
		l4k[i].base = i;
		l4k[i].p = 1;
		l4k[i].us = 0;
		l4k[i].rw = 1;
		l4k[i].ps = 1;
		l4k[i].exb = 1;
	}

	// Kernel global area(code, rodata, modules, gmalloc)
	l4k[1].exb = 0;

	// Kernel local area(malloc, TLB, TS, data, bss, stack)
	l4k[2 + apic_id] = l4k[2];

	l4k[2].base = 2 + apic_id;	// 2 * (2 + apic_id)MB
	l4k[2].p = 1;
	l4k[2].us = 0;
	l4k[2].rw = 1;
	l4k[2].ps = 1;

	if(apic_id == 0)
		pass();
}

static void activate_pml4(uint8_t apic_id) {
	uint32_t pml4 = 0x5c0000 + 0x200000 * apic_id;
	asm volatile("movl %0, %%cr3" : : "r"(pml4));

	if(apic_id == 0) {
		vga_print_string("Activate PML4 table: 0x");
		vga_print_u32(pml4);
		pass();
	}
}

static void activate_longmode(uint8_t apic_id) {
	#define LONGMODE	0x0901	// NXE=1, LME=1, SCE=1

	uint32_t msr;
	asm volatile("movl %1, %%ecx\n"
		"rdmsr\n"
		"movl %%eax, %0\n"
		: "=r"(msr)
		: "r"(0xc0000080));

	msr |= LONGMODE;

	asm volatile("movl %0, %%eax\n"
		"wrmsr"
		: : "r"(msr));

	if(apic_id == 0) {
		vga_print_string("Activate long-mode: 0x");
		vga_print_u32(msr);
		pass();
	}
}

static void activate_paging(uint8_t apic_id) {
	#define ADD		0xe000003e	// PG=1, CD=1, NW=1, NE=1, TS=1, EM=1, MP=1
	#define REMOVE	0x60000004	//       CD=1, NW=1,             EM=1

	uint32_t cr0;
	asm volatile("movl %%cr0, %0" : "=r"(cr0));
	cr0 |= ADD;
	cr0 ^= REMOVE;

	if(apic_id == 0) {
		vga_print_string("Activate caching and paging: 0x");
		vga_print_u32(cr0);
	}

	asm volatile("movl %0, %%cr0" : : "r"(cr0));

	if(apic_id == 0) {
		pass();
	}
}

void main(uint32_t magic, uint32_t addr) {
	vga_init();
	vga_print_string("PacketNgin ver 4.0\n\n");

	check_multiboot(magic, addr);
	check_cpuid();
	check_longmode();
	load_gdt(0);
	activate_pae(0);
	init_page_tables(0);
	activate_pml4(0);
	activate_longmode(0);
	activate_paging(0);

	return;
}
