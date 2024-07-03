#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

void log_error(char *format, ...);
void log_warning(char *format, ...);
void log_info(char *format, ...);
void skip_spaces(char **p);
void *safe_malloc(size_t size);
void *safe_realloc(void *ptr, size_t size);
void safe_free(void *ptr);
FILE *open_file(char *filename, char *mode);
void close_file(FILE *file);
int is_comment(char *p_line);
int is_start_of_macro(char *p_line);
int is_end_of_macro(char *p_line);
void copy_string_until_space(char *dest, const char *src);

#endif