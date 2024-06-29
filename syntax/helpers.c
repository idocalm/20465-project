#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
/* skip_spaces - move the pointer to the first non-space character */
void skip_spaces(char **p)
{
    while (**p == ' ' || **p == '\t')
    {
        (*p)++;
    }
}

FILE *open_file(const char *fileName, const char *mode)
{
    FILE *file = fopen(fileName, mode);
    if (file == NULL)
    {
        /* TODO: Check forum if program should exit */
        fprintf(stderr, "[ERROR]: Could not open file %s\n", fileName);
        exit(1);
    }
    return file;
}

void *alloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL)
    {
        /* TODO: Check forum if program should exit */
        fprintf(stderr, "[ERROR]: Memory allocation failed\n");
        exit(1);
    }
    return p;
}
/* copy_string - takes a destination string and copies everything from the source WITHOUT remaining spaces */
void copy_string_until_space(char *dest, const char *src)
{
    int i = 0, j = 0;
    while (!isspace(src[j]))
    {
        dest[i] = src[j];
        i++;
        j++;
    }
}