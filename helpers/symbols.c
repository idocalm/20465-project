#include "symbols.h"

/*
    ------- Symbols detection -------
*/

/**
    * Finds responding operation to a given string (if exists).
    * @param str - the string to search for.
    * @return the operation that corresponds to the string.
*/

Operation get_operation(char *str)
{
    int i;
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
    int length = sizeof(operations) / sizeof(operation_search);



    for (i = 0; i < length; i++)
    {
        /* Check if the string is equal to the directive name */
        if (strcmp(str, operations[i].name) == 0)
        {
            return operations[i].operation;
        }
    }

    /* Return not found */

    return UNKNOWN_OPERATION;

    
}

/**
    * Finds responding directive to a given string (if exists).
    * @param str - the string to search for.
    * @return the directive that corresponds to the string.
*/

Directive get_directive(char *str)
{
    int i; 
    directive_search directives[] = {
        {".data", DATA},
        {".string", STRING},
        {".entry", ENTRY},
        {".extern", EXTERN}
    };

    int length = sizeof(directives) / sizeof(directive_search);


    for (i = 0; i < length; i++)
    {
        /* Check if the string is equal to the directive name */
        if (strcmp(str, directives[i].name) == 0)
        {
            return directives[i].directive;
        }
    }

    /* Return not found */
    return UNKNOWN_DIRECTIVE;
}

/**
    * Finds responding register to a given string (if exists).
    * @param str - the string to search for.
    * @return the register that corresponds to the string.
*/
Register get_register(char *str)
{
    if (strlen(str) != 2) 
    {
        return UNKNOWN_REGISTER;
    }

    /* Check that the string is 2 characters long, with 'r' as the first character and then a digit */
    if (str[0] == 'r' && isdigit(str[1]))
    {
        int digit = str[1] - '0'; /* Convert the digit from ASCII to an integer */
        return digit >= 0 && digit <= 7 ? digit : UNKNOWN_REGISTER;
 
    }

    return UNKNOWN_REGISTER;
}
/*
void update_addressing_mode(AddressMode *mode, 
*/