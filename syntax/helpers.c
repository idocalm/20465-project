#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "helpers.h"
#include "../macros.h"

void red_text()
{
    printf("\033[0;31m");
}

void yellow_text()
{
    printf("\033[0;33m");
}

void reset_text()
{
    printf("\033[0m");
}

void log_error(char *format, ...)
{
    va_list args;
    red_text();
    printf("[ERROR] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    reset_text();
}

void log_warning(char *format, ...)
{
    va_list args;
    yellow_text();
    printf("[WARNING] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    reset_text();
}

void log_info(char *format, ...)
{
    va_list args;
    printf("[INFO] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

/* skip_spaces - move the pointer to the first non-space character */
void skip_spaces(char **p)
{
    while (isspace(**p))
    {
        (*p)++;
    }
}

void *safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        log_error("Could not allocate memory of size: %ld\n", size);
        exit(1);
    }
    return ptr;
}

void *safe_realloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);
    if (ptr == NULL)
    {
        log_error("Could not reallocate memory of size: %ld\n", size);
        exit(1);
    }
    return ptr;
}

void safe_free(void *ptr)
{
    if (ptr == NULL)
    {
        /*log_warning("Trying to free NULL pointer\n");*/
        return;
    }
    free(ptr);
}

FILE *open_file(const char *filename, char *mode)
{
    FILE *file = fopen(filename, mode);
    if (file == NULL)
    {
        log_error("Could not open file %s\n", filename);
        exit(1);
    }
    return file;
}

void close_file(FILE *file)
{
    if (file == NULL)
    {
        log_warning("Trying to close NULL file\n");
        return;
    }
    fclose(file);
}

int is_comment(char *p_line)
{
    return strncmp(p_line, COMMENT_PREFIX, COMMENT_PREFIX_LEN) == 0;
}

int is_macro_start(char *p_line)
{
    return strncmp(p_line, MACRO_START_PREFIX, MACRO_START_PREFIX_LEN) == 0;
}

int is_macro_end(char *p_line)
{
    return strncmp(p_line, MACRO_END_PREFIX, MACRO_END_PREFIX_LEN) == 0;
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

