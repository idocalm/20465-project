#ifndef MACROS_H
#define MACROS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#include "structs/list.h"
#include "definitions.h"
#include "helpers/symbols.h"
#include "helpers/strings.h"

#define MACRO_START_PREFIX "macr"
#define MACRO_START_PREFIX_LEN strlen(MACRO_START_PREFIX)
#define MACRO_END_PREFIX "endmacr"
#define MACRO_END_PREFIX_LEN strlen(MACRO_END_PREFIX)
#define COMMENT_PREFIX ';'

typedef enum {
    NO_MACRO_ERROR,
    MACRO_ERROR
} MacroError;

MacroError handle_macros(char *file_name);
void *search_in_macros(char *str, List *macros);
int validate_label(char *line, int type, List *macros, int line_num);

MacroError replace_macros(FILE *input_file, FILE *output_file, char *output_file_name);

#endif