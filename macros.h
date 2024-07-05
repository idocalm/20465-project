#ifndef MACROS_H
#define MACROS_H

#include "structs/hash_table.h"

#define MAX_LINE_SIZE 80
#define MACRO_START_PREFIX "macr"
#define MACRO_START_PREFIX_LEN strlen(MACRO_START_PREFIX)
#define MACRO_END_PREFIX "endmacr"
#define MACRO_END_PREFIX_LEN strlen(MACRO_END_PREFIX)
#define COMMENT_PREFIX ";"
#define COMMENT_PREFIX_LEN strlen(COMMENT_PREFIX)

enum MacroErorrs
{
    NO_ERROR,
    MULTIPLE_MACRO_DEFINITIONS,
    INVALID_MACRO_NAME,
    EXTRANEOUS_CHARACTERS,
    MACRO_NOT_FOUND
};



typedef struct
{
    int error;
    ht_t *macros;
} MacroResult;

MacroResult *replace_macros(const char *p_fileName);

#endif