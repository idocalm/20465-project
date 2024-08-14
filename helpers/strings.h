#ifndef STRINGS_H
#define STRINGS_H

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../definitions.h"
#include "./memory.h"


/* -- Prototypes -- */

void skip_spaces(char **p);
void remove_all_spaces(char *p);
char* copy_string_until_space(char *p);
int is_integer(char *p);


#endif