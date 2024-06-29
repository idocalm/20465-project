#ifndef MACROS_H
#define MACROS_H

#include "structs/hash_table.h"

#define MAX_LINE_SIZE 80
#define MACRO_START_PREFIX "macr"
#define MACRO_END_PREFIX "endmacr"

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
    HashTable *macros;
} MacroResult;

MacroResult *search_macros_in_file(const char *fileName);
int replace_macros_in_file(const char *fileName, HashTable *result);

#endif