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

typedef enum 
{
    NO_MACRO_ERROR,
    MACRO_ERROR
} MacroErrors;


int is_comment(char *p_line);
int handle_ignore_macros(char *p_line, int insideMacro);
void extract_file_content(char *p_fileName, char **pp_content);
MacroErrors handle_macros(char *p_fileName, List *p_macros);
MacroErrors extract_macros(FILE *p_file, List *p_macros);

void replace_macros(FILE *p_file, char *p_fileName, List *p_macros);


#endif