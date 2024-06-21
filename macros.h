#include "syntax.h"


#define MAX_LINE_LENGTH 80
#define MACRO_START_KEYWORD "macr"
#define MACRO_END_KEYWORD "endmacr"

struct macro { 
    char name[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH]; 
    struct macro *next; 
};

/* Prototypes */

struct macro *isMacroDefinition(char *line);
void fillMacroValue(FILE *file, struct macro *macro);
int scanMacros(char *fileName, struct macro *macro);
void replaceMacros(char *fileName, struct macro *macroHead);
