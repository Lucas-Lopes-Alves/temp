#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "terminal.h"
#include "functions.h"
#include "io.h"

// list all the possible colors in vga mode
enum vga_color
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

// function for setting the color in the terminal in the vga mode
// returns a size_t with the value of the foreground and background
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | (bg << 4);
}

static inline uint16_t vga_entry(unsigned char c, uint8_t color)
{
    return (uint16_t)c | (uint16_t)(color << 8);
}

// VGA_MEMORY is the phisical adress to the
// the vga buffer memory
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000


size_t terminal_row = 0;
size_t terminal_column = 0;
uint8_t terminal_color;

size_t initial_column = 0;
size_t initial_row = 0;
char line[2000] = {0};
char command[100] = {0};


// The terminal_buffer set as volatile to prevent otimizations
// that can ignore the acess to the memory
volatile uint16_t *terminal_buffer = (volatile uint16_t *)VGA_MEMORY;
#define VGA_BUFFER ((volatile uint16_t(*)[80])terminal_buffer)

static inline void prompt()
{
    uint8_t temp = terminal_color;
    terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK));
    vga_writestring("tesnix> ");
    initial_column = terminal_column;
    initial_row = terminal_row;
    terminal_setcolor(temp);
}

// it's two loops to acess all the adresses in the buffer
// and clean them, it acess like a matrix using rows and columns
// but translates it to 1D and saves in the index variable
void terminal_initialize(void)
{
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    uint16_t caractere = vga_entry(' ', terminal_color);
    for (size_t i = 0; i < VGA_HEIGHT; i++)
    {
        for (size_t j = 0; j < VGA_WIDTH; j++)
        {
            VGA_BUFFER[i][j] = caractere;
        }
    }

    prompt();
}

void clear()
{
    for (size_t i = 0; i < VGA_HEIGHT; i++)
    {
        for (size_t j = 0; j < VGA_WIDTH; j++)
        {
            VGA_BUFFER[i][j] = vga_entry(' ', terminal_color);
        }
    }

    terminal_column = 0;
    terminal_row = 0;
    prompt();
    update_cursor(terminal_row, terminal_column);
}

/*
 * Executes the "echo" command in the terminal, ignoring the initial word echo
 * (finding an space character ' '), this function jumps to the next line and 
 * print the rest of the text directly in the vga text buffer, handles the end
 * of the screen scrolling the terminal and update the terminal cursor
 */
void echo()
{
    // go to the start of the next line
    terminal_row += 1;
    terminal_column = 0;
    size_t character = 0;

    // add to the index to ignore the word echo
    while (line[character] != ' ')
    {
        character++;
    }
    character++; //increment to ignore the first space

    // print text in the vga buffer while the character is diferent from '\0'
    while (line[character])
    {
        // checks if it's in the last line of the buffer
        if (terminal_row > VGA_HEIGHT)
        {
            terminal_scroll();
        }

        // write directly in the buffer with text + color
        VGA_BUFFER[terminal_row][terminal_column] = vga_entry(line[character],terminal_color);
        terminal_column++;

        // if it reaches the horizontal limit of the buffer, it goes start of the next line
        if (terminal_column > VGA_WIDTH)
        {
            terminal_row++;
            terminal_column = 0;
        }
        character++;
    }

    // configure the enviroment to the next command
    terminal_row+=1;
    terminal_column = 0;
    prompt(); // print the command indicator (tesnix> )

    // update the cursor to the actual position of the
    update_cursor(terminal_row, terminal_column);
    return;
}

void update_cursor(size_t y, size_t x)
{
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

// scroll the terminal up by moving each line and
// cleaning the last lines
void terminal_scroll()
{
    for (size_t i = 1; i < VGA_HEIGHT; i++)
    {
        for (size_t j = 0; j < VGA_WIDTH; j++)
        {
            VGA_BUFFER[i-1][j] = VGA_BUFFER[i][j];
        }
    }

    for (size_t k = 0; k < VGA_WIDTH; k++)
    {
        VGA_BUFFER[24][k] = vga_entry(' ', terminal_color);
    }
    terminal_row = VGA_HEIGHT - 1;
    initial_row -= 1;
}

/*
 *  Writes a single character to the vga buffer
 *
 *  Handles:
 *      The '\n' character to jump to a new line
 *      If the line is the last it Triggers the Scrolls
 *      If the column is the last, it goes to a new line and the start of a new line
 */
void vga_putchar(char c)
{
    if (terminal_row >= VGA_HEIGHT)
    {
        terminal_scroll();
    }
    if (c == '\n')
    {
        detect_command();
        if (kstrcmp(command,"clear"))
        {
            clear();
            return;
        }
        if (kstrcmp(command,"echo"))
        {
            echo();
            return;
        }
        terminal_row++;
        terminal_column = 0;
        prompt();

        if (terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
        return;
    }
    if (c == '\b')
    {
        if (terminal_column == initial_column && terminal_row == initial_row)
        {
            return;
        }
        if (terminal_column - 1 > VGA_WIDTH)
        {
            terminal_row -= 1;
            terminal_column = VGA_WIDTH;
        } else
        {
            terminal_column = terminal_column - 1;
        }
        VGA_BUFFER[terminal_row][terminal_column] = vga_entry(' ', terminal_color);
        return;
    }
    if (c == '\t')
    {
        for (size_t i = 0; i != 4; i++)
        {
            vga_putchar(' ');
        }
        return;
    }
    VGA_BUFFER[terminal_row][terminal_column] = vga_entry(c,terminal_color);
    terminal_column++;
    if (terminal_column == VGA_WIDTH)
    {
        terminal_row++;
        terminal_column = 0;
    }
}

// function that writes in the screen and go to the next line
// when find the '\n' character, if it detects it's in the last line
// it scrolls the terminal up by invocating the terminal_scroll function
void vga_write(const char *info, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        vga_putchar(info[i]);
    }
}

// Invokes the vga_write and handles the string's length
void vga_writestring(const char *string)
{
    vga_write(string, kstrlen(string));
    update_cursor(terminal_row, terminal_column);
}

/*

*/
void detect_command()
{
    size_t count = 0;
    size_t initial_index = (initial_row * VGA_WIDTH) + initial_column;
    size_t actual_index = (terminal_row * VGA_WIDTH) + terminal_column;
    count = initial_index;
    size_t temp = 0;
    while((char)terminal_buffer[count] != ' '){
        command[temp] = (char)terminal_buffer[count];
        count++;
        temp++;
    }
    temp++;
    command[temp] = '\0';
    count = 0;
    for (size_t i = initial_index; i < actual_index; i++, count++)
    {
        line[count] = (char)terminal_buffer[i];
    }
    line[count] = '\0';
}