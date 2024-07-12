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

void green_text()
{
    printf("\033[0;32m");
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

void log_success(char *format, ...)
{
    va_list args;
    green_text();
    printf("[SUCCESS] ");
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

void skip_non_spaces(char **pp_line) {
    while (**pp_line && !isspace(**pp_line)) {
        (*pp_line)++;
    }
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

    dest[i] = '\0';



}



void extract_file_content(char *p_fileName, char **pp_content) {
    FILE *p_file = NULL;
    long length;

    p_file = open_file(p_fileName, "r");

    fseek(p_file, 0, SEEK_END);
    length = ftell(p_file);
    rewind(p_file);

    *pp_content = (char *)safe_malloc(length + 1);

    fread(*pp_content, 1, length, p_file);
    (*pp_content)[length] = '\0';

    close_file(p_file);
}



int non_character(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\0';
}