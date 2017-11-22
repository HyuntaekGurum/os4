#ifndef __VGA_H__
#define __VGA_H__

#define VGA_NORMAL  0x07

/**
 * Init video driver
 */
void vga_init();

/**
 * Get VGA cursor index.
 *
 * @return cursor index
 */
uint16_t vga_get_cursor();

/**
 * Set VGA cursor index.
 *
 * @param idx cursor index
 */
void vga_set_cursor(uint16_t idx);

/**
 * Set VGA color attribute.
 *
 * @param attr color attribute
 */
void vga_set_attr(uint8_t attr);

/**
 * Print string value to VGA.
 *
 * @param str string value
 */
void vga_print_string(const char* str);

/**
 * Print unsigned 8 integer to VGA.
 *
 * @param v unsigned 8 integer value
 */
void vga_print_u8(uint8_t v);

/**
 * Print unsigned 16 integer to VGA.
 *
 * @param v unsigned 16 integer value
 */
void vga_print_u16(uint16_t v);

/**
 * Print unsigned 32 integer to VGA.
 *
 * @param v unsigned 32 integer value
 */
void vga_print_u32(uint32_t v);

/**
 * Print unsigned 64 integer to VGA.
 *
 * @param v unsigned 64 integer value
 */
void vga_print_u64(uint64_t v);

/**
 * Move the cursor to specific column.
 *
 * @param col column index
 */
void vga_tab(uint32_t col);

#endif /* __VGA_H__ */
