#ifndef __KEYBOARD__
#define __KEYBOARD__

#include <stdint.h>

void keyboard_scancode(uint8_t key, char* out);
char keyboard_char(uint8_t scancode);

#endif