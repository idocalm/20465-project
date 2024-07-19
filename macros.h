#ifndef MACROS_H
#define MACROS_H

#include "structs/hash_table.h"

#define MACRO_START_PREFIX "macr"
#define MACRO_START_PREFIX_LEN strlen(MACRO_START_PREFIX)
#define MACRO_END_PREFIX "endmacr"
#define MACRO_END_PREFIX_LEN strlen(MACRO_END_PREFIX)
#define COMMENT_PREFIX ';'

typedef enum 
{
    NO_MACRO_ERROR,
    MULTIPLE_MACRO_DEFINITIONS,
    INVALID_MACRO_NAME,
    EXTRANEOUS_CHARACTERS,
    MACRO_NOT_FOUND,
    INVALID_FILE_EXTENSION,
    NO_MACRO_NAME,
    LINE_LENGTH_EXCEEDED
} MacroErrors;


int is_comment(char *p_line);
int handle_ignore_macros(char *p_line, int insideMacro);
void extract_file_content(char *p_fileName, char **pp_content);
MacroErrors handle_macros(char *p_fileName, ht_t *p_macros);
MacroErrors extract_macros(char *p_fileName, ht_t *p_macros);

char *replace_macros(char *content,  ht_t *p_macros);


#endif