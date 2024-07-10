#include <stdlib.h>

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "symbols.h"


Operation get_operation(char *str)
{
    
    operation_search operations[] = {
        {"mov", MOV},
        {"cmp", CMP},
        {"add", ADD},
        {"sub", SUB},
        {"not", NOT},
        {"clr", CLR},
        {"lea", LEA},
        {"inc", INC},
        {"dec", DEC},
        {"jmp", JMP},
        {"bne", BNE},
        {"red", RED},
        {"prn", PRN},
        {"jsr", JSR},
        {"rts", RTS},
        {"stop", STOP}
    };


    int i = 0;

    for (; i < sizeof(operations) / sizeof(operation_search); i++)
    {
        if (strcmp(str, operations[i].name) == 0)
        {
            return operations[i].operation;
        }
    }

    return UNKNOWN_OPERATION;

    
}

Directive get_directive(char *str)
{
    directive_search directives[] = {
        {".data", DATA},
        {".string", STRING},
        {".entry", ENTRY},
        {".extern", EXTERN}
    };

    int i = 0;

    for (; i < sizeof(directives) / sizeof(directive_search); i++)
    {
        if (strcmp(str, directives[i].name) == 0)
        {
            return directives[i].directive;
        }
    }


    return UNKNOWN_DIRECTIVE;
}

Register get_register(char *str)
{
    if (strlen(str) != 3)
    {
        return UNKNOWN_REGISTER;
    }

    /* Check that the string is 2 characters long, with 'r' as the first character and then a digit */
    if (str[0] == 'r' && isdigit(str[1]) && str[2] == '\0')
    {
        int digit = str[1] - '0'; /* Convert the digit from ASCII to an integer */
        return digit >= 0 && digit <= 7 ? digit : UNKNOWN_REGISTER;
 
    }

    return UNKNOWN_REGISTER;
}