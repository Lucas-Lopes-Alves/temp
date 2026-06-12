#include "io.h"
#include <stdint.h>
#include <stdbool.h>
#include "keymap.h"

void ps2_write_wait()
{
    while (inb(0x64) & 0x02)
        ;
}

void ps2_read_wait()
{
    while (!(inb(0x64) & 0x01))
        ;
}

void keyboard_scancode(uint8_t port, char *out)
{
    uint8_t key = inb(port);
    const char digits[] = "0123456789ABCDEF";
    out[0] = digits[(key >> 4) & 0xF];
    out[1] = digits[key & 0xF];
    out[2] = '\0';
}

typedef struct locks
{
    bool capslock;
    bool scrollock;
    bool numlock;
}locks;

locks kbd_state = {false,false,false};

bool shift = false;

const unsigned char* ptr = chars;

char keyboard_char(uint8_t scancode)
{
    if (scancode == 0x2A || scancode == 0x36)
    {
        shift = true;
        return '\0';
    }

    if (scancode == 0x2A + 0x80 || scancode== 0x36 + 0x80)
    {
        shift = false;
        return '\0';
    }

    if (scancode == 0x3A)
    {
        kbd_state.capslock = !kbd_state.capslock;
        ps2_write_wait();
        outb(0x60, 0xED);
        inb(0x60);
        ps2_write_wait();
        outb(0x60, kbd_state.capslock ? 0x00 : 0x04);
        return '\0';
    }

    if (scancode & 0x80)
    {
        return '\0';
    }

    if (kbd_state.capslock != shift)
    {
        ptr = charsAlt;
    }
    else
    {
        ptr = chars;
    }

    return ptr[scancode];
}
