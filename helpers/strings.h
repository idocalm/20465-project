#ifndef STRINGS_H
#define STRINGS_H

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../definitions.h"
#include "./memory.h"

#define COMMENT_PREFIX ';'

/* -- Prototypes -- */

void skip_spaces(char **p);
void remove_all_spaces(char *line);
char* copy_string_until_space(const char *src);
int is_comment(char *line);
int is_integer(char *p);


#endif