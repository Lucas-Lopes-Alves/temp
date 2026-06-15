#include "terminal.h"
#include "keyboard.h"
#include "io.h"

void kernel_main(void)
{
    terminal_initialize();
    while (1)
    {
        if (inb(0x64) & 0x01)
        {
            uint8_t code = inb(0x60);
            char str[2] ={keyboard_char(code), '\0'};
            if (str[0] == '\0'){ continue;}
            vga_writestring(str);
        }
        // char str[3];
        // keyboard_scancode(0x60,str);
        // vga_writestring(str);
        // vga_writestring("\n");
    }
}