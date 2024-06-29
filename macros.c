#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "macros.h"
#include "syntax/helpers.h"
#include "syntax/symbols.h"

MacroResult *search_macros_in_file(const char *fileName)
{
    char *line_start = NULL;  /* Pointer to the start of the line */
    char line[MAX_LINE_SIZE]; /* Buffer to store the line */
    int is_in_macro = 0;      /* Flag to indicate if we are inside a macro */
    char *macroValue = NULL;  /* Buffer to store the current macro value */
    char *macroName = NULL;   /* Buffer to store the current macro name */
    HashTable *macros = NULL; /* Hash table to store the macros */
    FILE *file = NULL;        /* File pointer */
    MacroResult *result = NULL;
    printf("Searching for macros in file %s\n", fileName);
    file = open_file(fileName, "r");

    result = alloc(sizeof(MacroResult));
    result->error = NO_ERROR;

    macros = hashtable_init(); /* Create a new hash table to store the macros */
    result->macros = macros;

    while (fgets(line, MAX_LINE_SIZE, file) != NULL)
    {
        line_start = line;

        /* Skip any spaces at the beginning of the line */
        skip_spaces(&line_start);
        /* Check if the line starts with the macro start prefix */
        if (strncmp(line_start, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0)
        {
            /* Skip the macro start prefix */
            line_start += strlen(MACRO_START_PREFIX);

            /* Skip any spaces after the macro start prefix */
            skip_spaces(&line_start);

            /* Create a new char array to store the macro name, which will be the first word in the line */
            macroName = (char *)alloc(strlen(line_start));
            copy_string_until_space(macroName, line_start);
            macroName = (char *)realloc(macroName, strlen(macroName));

            /* Skip any spaces after the macro name */
            skip_spaces(&line_start);

            /* Check that the name is not an operation or a register */

            if (is_operation(macroName) || is_register(macroName) || is_directive(macroName))
            {
                result->error = INVALID_MACRO_NAME;
                return result;
            }

            /* Check if the macro is already defined */
            if (hashtable_getstr(macros, macroName) != NULL)
            {
                result->error = MULTIPLE_MACRO_DEFINITIONS;
                return result;
            }

            is_in_macro = 1;
        }
        else if (is_in_macro)
        {
            /* Check if the line starts with the macro end prefix */

            if (strncmp(line_start, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0)
            {
                is_in_macro = 0;
                /* Add the macro value to the hash table */
                hashtable_putstr(macros, macroName, macroValue);

                /* Reset the macro value */
                free(macroValue);
                free(macroName);

                macroValue = NULL;
            }
            else
            {

                /* Add the line to the macro value */
                if (macroValue == NULL)
                {
                    macroValue = (char *)alloc(strlen(line_start));
                    strcpy(macroValue, line_start);
                    macroValue = (char *)realloc(macroValue, strlen(macroValue));
                }
                else
                {
                    macroValue = (char *)realloc(macroValue, strlen(macroValue) + strlen(line_start));
                    strcat(macroValue, line_start);
                    macroValue = (char *)realloc(macroValue, strlen(macroValue));
                }
            }
        }
    }

    fclose(file);
    return result;
}

int replace_macros_in_file(const char *fileName, HashTable *result)
{
    char *line_start = NULL;  /* Pointer to the start of the line */
    char line[MAX_LINE_SIZE]; /* Buffer to store the line */
    char *newFileName = NULL; /* Buffer to store the new file name */
    FILE *file = NULL;        /* File pointer */
    FILE *newFile = NULL;     /* File pointer for the new file */
    char *macroValue = NULL;  /* Buffer to store the current macro value */
    char *macroName = NULL;   /* Buffer to store the current macro name */
    int is_in_macro = 0;      /* Flag to indicate if we are inside a macro */
    printf("Replacing macros in file %s\n", fileName);
    newFileName = (char *)alloc(strlen(fileName));
    file = open_file(fileName, "r");
    strcpy(newFileName, fileName);
    newFileName[strlen(newFileName) - 2] = 'a';
    newFileName[strlen(newFileName) - 1] = 'm';

    newFile = open_file(newFileName, "w");

    while (fgets(line, MAX_LINE_SIZE, file) != NULL)
    {
        line_start = line;
        skip_spaces(&line_start);
        if (strcmp(line_start, "\n") == 0 ||
            strcmp(line_start, "\r\n") == 0 ||
            strcmp(line_start, "\0") == 0)
        {

            /* fprintf(newFile, "\n"); */
            continue;
        }

        if (strncmp(line_start, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0)
        {
            is_in_macro = 1;
            continue;
        }
        else if (is_in_macro && strncmp(line_start, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0)
        {
            is_in_macro = 0;

            continue;
        }
        if (!is_in_macro && !is_directive(line_start) && !is_operation(line_start) && !is_register(line_start))
        {
            printf("Not directive, operation or register\n");
            macroName = (char *)alloc(strlen(line_start));
            copy_string_until_space(macroName, line_start);
            macroName = (char *)realloc(macroName, strlen(macroName));
            printf("Macro name: %s\n", macroName);
            macroValue = hashtable_getstr(result, macroName);
            printf("Macro value: %s\n", macroValue);
            if (macroValue != NULL)
            {
                fprintf(newFile, "%s", macroValue);
            }
            else
            {
                fprintf(newFile, "%s", line_start);
            }
            free(macroName);
        }
        else if (!is_in_macro)
        {
            fprintf(newFile, "%s", line_start);
        }
    }

    printf("Finished replacing macros\n");

    free(newFileName);

    fclose(file);
    fclose(newFile);

    return 0;
}