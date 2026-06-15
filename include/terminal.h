#ifndef VGA_TERMINAL
#define VGA_TERMINAL

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_scroll();
void vga_putchar(char c);
void vga_write(const char *info, size_t size);
void vga_writestring(const char *string);
void detect_command();
void update_cursor(size_t y, size_t x);


#endif