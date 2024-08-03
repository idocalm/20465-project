#ifndef STRINGS_H
#define STRINGS_H

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../globals.h"

#define COMMENT_PREFIX ';'
#define MIN_12_BIT_NUMBER -2048
#define MAX_12_BIT_NUMBER 2047

void skip_non_spaces(char **p);
void skip_spaces(char **p);

int is_empty(char *str);

void remove_all_spaces(char *line);
void copy_string_until_space(char *dest, const char *src);
int non_character(char c);
int is_comment(char *line);
int is_integer(char *p);

#endif