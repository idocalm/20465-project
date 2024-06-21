/* This file contains different functions that are used to validate and find errors in the syntax of the assembly code */

#include "syntax.h"

/* This function skips over spaces and tabs in a string */
void skipSpaces(char *line, int *i) {
    while (line[*i] == ' ' || line[*i] == '\t') {
        (*i)++;
    }
}


/* This function validates a macro name */

int validateMacroName(char *name) {

    const char *reservedWords[] = {
        "mov", "cmp", "add", "sub", 
        "lea", "clr", "not", "inc", 
        "dec", "jmp", "bne", "red",
        "prn", "jsr", "rts", "stop",

        /* TODO: Are register names reserved? */

        "r0", "r1", "r2", "r3", "r4", "r5"


    };

    int i = 0;

    while (*reservedWords[i] != NULL) {
        if (strcmp(name, *reservedWords[i]) == 0) {
            return UNVALID_MACRO_NAME;
        }
        i++;
    }

    return 0;
}