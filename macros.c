#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "macros.h"
#include "syntax/helpers.h"
#include "syntax/symbols.h"

void trim_line(char *p_line)
{
    int i = 0;
    while (!isspace(p_line[i]))
    {
        i++;
    }
    p_line[i] = '\0';
}

int get_word_length(char *p_line)
{
    int i = 0;
    while (!isspace(p_line[i]))
    {
        i++;
    }
    return i;
}

char *get_word(char *p_line)
{
    int wordLength = get_word_length(p_line);
    char *p_word = (char *)safe_malloc(wordLength);
    int i = 0;
    for (; i < wordLength; i++)
    {
        p_word[i] = p_line[i];
    }
    p_word[i] = '\0';
    return p_word;
}

int find_end_of_line(char *p_line)
{
    int i = 0;
    while (p_line[i] != '\0')
    {
        i++;
    }
    return i;
}

int find_macro_name_length(char *p_line)
{
    int i = 0;
    while (!isspace(p_line[i]))
    {
        i++;
    }
    return i;
}

char *find_macro_name(char **pp_line)
{
    int macroNameLength = find_macro_name_length(*pp_line);
    char *p_macroName = (char *)safe_malloc(macroNameLength);
    int i = 0;
    while (!isspace(**pp_line))
    {
        p_macroName[i++] = **pp_line;
        (*pp_line)++;
    }
    p_macroName[i] = '\0';
    p_macroName = safe_realloc(p_macroName, i + 1);
    /* clearing the line */
    skip_spaces(pp_line);
    return p_macroName;
}

MacroResult *search_macros_in_file(const char *fileName)
{
    int lineCount = 0;
    ht_t *p_macros = ht_create();
    char *p_line = NULL;
    char line[MAX_LINE_LENGTH];
    char *p_macroName = NULL;
    char *p_macroValue = NULL;
    char *p_newFileName = NULL;
    FILE *p_file = NULL;
    FILE *p_newFile = NULL;
    int isInMacro = 0;
    MacroResult *result = NULL;

    log_info("Searching for macros in file %s\n", p_fileName);
    p_file = open_file(p_fileName, "r");

    result = safe_malloc(sizeof(MacroResult));
    result->error = NO_ERROR;
    result->macros = p_macros;

     /* creating new file */
    p_newFileName = (char *)safe_malloc(strlen(p_fileName));
    strcpy(p_newFileName, p_fileName);
    p_newFileName[strlen(p_newFileName) - 1] = 'm';
    p_newFile = open_file(p_newFileName, "w");
    /* -----------------*/

    while (fgets(line, MAX_LINE_SIZE, p_file) != NULL)
    {
        lineCount++;
        p_line = line;

        /* Skip any spaces at the beginning of the line */
        skip_spaces(&p_line);
        /* skip blank line */
        if (*p_line == '\0' || is_comment(p_line))
        {
            fprintf(p_newFile, "\n");
            continue;
        }
        /* Check if the line starts with the macro start prefix */
        if (!isInMacro && is_macro_start(p_line))
        {
            /* Skip the macro start prefix */
            p_line += MACRO_START_PREFIX_LEN;

            /* Skip any spaces after the macro start prefix */
            skip_spaces(&p_line);

            /* Create a new char array to store the macro name, which will be the first word in the line */
            p_macroName = find_macro_name(&p_line);

            /* Skip any spaces after the macro name */
            skip_spaces(&p_line);

            /* Check that the name is not an operation or a register */

            if (is_operation(p_macroName) || is_register(p_macroName) || is_directive(p_macroName))
            {
                result->error = INVALID_MACRO_NAME;
                goto clean;
            }

            if (strlen(p_line) > 0 && !is_comment(p_line))
            {
                log_error("Extra characters after macro name\n\tLine %d: unexpected token: %s\n", lineCount, p_line);
                result->error = EXTRANEOUS_CHARACTERS;
                goto clean;
            }

            /* Check if the macro is already defined */
            if (ht_get(p_macros, p_macroName) != NULL)
            {
                result->error = MULTIPLE_MACRO_DEFINITIONS;
                goto clean;
            }

            isInMacro = 1;
            continue;
        }
        if (isInMacro && !is_macro_end(p_line))
        {
            int lineLength = find_end_of_line(p_line);
            if (p_macroValue == NULL)
            {
                p_macroValue = (char *)safe_malloc(lineLength);
                strcpy(p_macroValue, p_line);
            }
            else
            {
                p_macroValue = (char *)safe_realloc(p_macroValue, strlen(p_macroValue) + lineLength);
                strcat(p_macroValue, p_line);
            }
            continue;
        }
        if (isInMacro && is_macro_end(p_line))
        {
            p_macroValue[strlen(p_macroValue)] = '\0';
            ht_set(p_macros, p_macroName, p_macroValue);
            isInMacro = 0;
            safe_free(p_macroValue);
            safe_free(p_macroName);
            p_macroValue = NULL;
            p_macroName = NULL;
            continue;
        }
        if (!isInMacro)
        {
            char *word = get_word(p_line);
            if (!is_operation(word))
            {
                trim_line(p_line);
                char *p_macroValueGet = ht_get(p_macros, p_line);
                if (p_macroValueGet != NULL)
                {
                    fprintf(p_newFile, "%s", p_macroValueGet);
                }
                else
                {
                    log_error("Invalid macro name: %s at line %d\n", p_line, lineCount);
                    result->error = MACRO_NOT_FOUND;
                    goto clean;
                }
            }
            else
            {
                fprintf(p_newFile, "%s", line);
            }
            safe_free(word);
        }
    }
clean:
    safe_free(p_macroName);
    safe_free(p_macroValue);
    safe_free(p_newFileName);
    ht_free(p_macros);
    close_file(p_file);
    close_file(p_newFile);
    log_info("Done searching for macros in file %s\n", p_fileName);
    return result;
}

int replace_macros_in_file(const char *fileName, HashTable *result)
{
    char *p_line = NULL;  /* Pointer to the start of the line */
    char line[MAX_LINE_SIZE]; /* Buffer to store the line */
    char *newFileName = NULL; /* Buffer to store the new file name */
    FILE *file = NULL;        /* File pointer */
    FILE *newFile = NULL;     /* File pointer for the new file */
    char *macroValue = NULL;  /* Buffer to store the current macro value */
    char *macroName = NULL;   /* Buffer to store the current macro name */
    int isInMacro = 0;      /* Flag to indicate if we are inside a macro */
    printf("Replacing macros in file %s\n", fileName);
    newFileName = (char *)alloc(strlen(fileName));
    file = open_file(fileName, "r");
    strcpy(newFileName, fileName);
    newFileName[strlen(newFileName) - 2] = 'a';
    newFileName[strlen(newFileName) - 1] = 'm';

    newFile = open_file(newFileName, "w");

    while (fgets(line, MAX_LINE_SIZE, file) != NULL)
    {
        p_line = line;
        skip_spaces(&p_line);
        if (strcmp(p_line, "\n") == 0 ||
            strcmp(p_line, "\r\n") == 0 ||
            strcmp(p_line, "\0") == 0)
        {

            /* fprintf(newFile, "\n"); */
            continue;
        }

        if (strncmp(p_line, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0)
        {
            isInMacro = 1;
            continue;
        }
        else if (isInMacro && strncmp(p_line, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0)
        {
            isInMacro = 0;

            continue;
        }
        if (!isInMacro && !is_directive(p_line) && !is_operation(p_line) && !is_register(p_line))
        {
            printf("Not directive, operation or register\n");
            macroName = (char *)alloc(strlen(p_line));
            copy_string_until_space(macroName, p_line);
            macroName = (char *)realloc(macroName, strlen(macroName));
            printf("Macro name: %s\n", macroName);
            macroValue = ht_get(result, macroName);
            printf("Macro value: %s\n", macroValue);
            if (macroValue != NULL)
            {
                fprintf(newFile, "%s", macroValue);
            }
            else
            {
                fprintf(newFile, "%s", p_line);
            }
            free(macroName);
        }
        else if (!isInMacro)
        {
            fprintf(newFile, "%s", p_line);
        }
    }

    printf("Finished replacing macros\n");

    free(newFileName);

    fclose(file);
    fclose(newFile);

    return 0;
}