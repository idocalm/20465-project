#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
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

            /*TODO: fix macro name, note: without newline*/
            /* Get the macro name */
            macroName = (char *)alloc(strlen(line_start) + 1);
            strcpy(macroName, line_start);

            /* Find the end of the macro name */
            while (((*line_start != ' ') && (*line_start != '\t')) && (*line_start != '\n'))
            {
                line_start++;
            }

            /* Make sure there is a space after the macro name */
            if (((*line_start != ' ') && (*line_start != '\t')) || (*line_start == '\n'))
            {
                result->error = EXTRANEOUS_CHARACTERS;
                return result;
            }
            /* Null-terminate the macro name */
            *line_start = '\0';

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

            /* Add the macro to the hash table */
            hashtable_putstr(macros, macroName, macroValue);

            is_in_macro = 1;
        }
        else if (is_in_macro)
        {
            /* Skip any spaces at the beginning of the line */
            skip_spaces(&line_start);
            /* Check if the line starts with the macro end prefix */
            if (strncmp(line_start, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0)
            {
                is_in_macro = 0;

                printf("Macro %s\n%s\n", macroName, macroValue);
                /* Add the macro value to the hash table */
                hashtable_putstr(macros, macroName, macroValue);
            }
            else
            {
                /* Add the line to the macro value */
                if (macroValue == NULL)
                {
                    macroValue = (char *)alloc(strlen(line_start) + 1);
                    printf("Setting Macro value: %s\n", line_start);
                    strcpy(macroValue, line_start);
                }
                else
                {
                    macroValue = (char *)realloc(macroValue, strlen(macroValue) + strlen(line_start) + 1);
                    printf("Appending Macro value: %s\n", line_start);
                    strcat(macroValue, line_start);
                }
            }
        }
    }

    fclose(file);
    return result;
}

int replace_macros_in_file(const char *fileName, HashTable *result)
{
    FILE *file = fopen(fileName, "r");
    char *newFileName = (char *)alloc(strlen(fileName));
    strcpy(newFileName, fileName);
    /*TODO: improve file extantion changing*/
    newFileName[strlen(fileName) - 2] = 'a';
    newFileName[strlen(fileName) - 1] = 'm';
    printf("Creating new file %s\n", newFileName);

    FILE *newFile = fopen(newFileName, "w");

    char *line_start = NULL;
    char line[MAX_LINE_SIZE];
    int is_in_macro = 0;

    /* Go over the result macros and replace them in the file every time you see the macro name, when it's NOT the actual definition */

    while (fgets(line, MAX_LINE_SIZE, file) != NULL)
    {
        line_start = line;
        skip_spaces(&line_start);

        if (strncmp(line_start, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0)
        {
            is_in_macro = 1;
            continue;
        }
        else if (strncmp(line_start, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0)
        {
            is_in_macro = 0;
            continue;
        }

        if (is_in_macro)
        {
            continue;
        }
        printf("key: %s\nvalue: %s", line, hashtable_getstr(result, line_start));
        if (hashtable_getstr(result, line_start) != NULL)
        {
            fprintf(newFile, "%s", hashtable_getstr(result, line_start));
        }
        else
        {
            fprintf(newFile, "%s", line);
        }
    }
}