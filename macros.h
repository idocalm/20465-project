#ifndef MACROS_H
#define MACROS_H

#include "structs/hash_table.h"

#define MACRO_START_PREFIX "macr"
#define MACRO_START_PREFIX_LEN strlen(MACRO_START_PREFIX)
#define MACRO_END_PREFIX "endmacr"
#define MACRO_END_PREFIX_LEN strlen(MACRO_END_PREFIX)
#define COMMENT_PREFIX ";"
#define COMMENT_PREFIX_LEN strlen(COMMENT_PREFIX)

typedef enum 
{
    NO_ERROR,
    MULTIPLE_MACRO_DEFINITIONS,
    INVALID_MACRO_NAME,
    EXTRANEOUS_CHARACTERS,
    MACRO_NOT_FOUND,
    INVALID_FILE_EXTENSION,
    NO_MACRO_NAME
} MacroErrors;




void extract_file_content(const char *p_fileName, char **pp_content);
MacroErrors handle_macros(const char *p_fileName);
MacroErrors extract_macros(const char *p_fileName, ht_t *p_macros);

char *replace_macros(const char *content,  ht_t *p_macros);


#endif