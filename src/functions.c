#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "functions.h"
#include "terminal.h"

size_t kstrlen(const char *str)
{
    size_t size = 0;
    while (str[size])
    {
        size++;
    }
    return size;
}

bool kstrcmp(const char* str1, const char* str2)
{
    while(*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return (*str2=='\0');
}

// char* kstrcat(char* str1,char* str2)
// {
    
// }