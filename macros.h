#include "syntax/errors.h"
#include "syntax/symbols.h"

#define MAX_LINE_SIZE 80
#define MACRO_START_PREFIX "macr"
#define MACRO_END_PREFIX "endmacr"

struct MacroResult {
    MacroError error;
    HashTable *macros;    
}