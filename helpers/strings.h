#ifndef STRINGS_H
#define STRINGS_H

#include <ctype.h>
#include <string.h>

#define COMMENT_PREFIX ';'

void skip_non_spaces(char **p);
void skip_spaces(char **p);

int is_empty(char *start, char *end);

void remove_all_spaces(char *line);
void copy_string_until_space(char *dest, const char *src);
int non_character(char c);
int is_comment(char *line);

#endif