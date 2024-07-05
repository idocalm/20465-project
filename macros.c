#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#define MAX_LINE_SIZE 80

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

void macros_exit_with_clean(ht_t *p_macros, char *p_macroName, char *p_macroValue, char *p_newFileName, FILE *p_file, FILE *p_newFile) {
    safe_free(p_macroName);
    safe_free(p_macroValue);
    safe_free(p_newFileName);
    ht_free(p_macros);
    close_file(p_file);
    close_file(p_newFile);
}


MacroResult *replace_macros(const char *p_fileName)
{
    int lineCount = 0;
    ht_t *p_macros = ht_create();
    char *p_line = NULL;
    char line[MAX_LINE_SIZE];
    char *p_macroName = NULL;
    char *p_macroValue = NULL;
    char *p_newFileName = NULL;
    char *p_macroValueGet = NULL;
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
                
                log_error("Invalid macro name: %s\n\tLine %d: Macro name is a reserved token: %s\n", p_macroName, lineCount, p_macroName);
                result->error = INVALID_MACRO_NAME;
                macros_exit_with_clean(p_macros, p_macroName, p_macroValue, p_newFileName, p_file, p_newFile);
                return result;
            }


            /* Check if there are any extra characters after the macro name */
            if (strlen(p_line) > 0 && !is_comment(p_line))
            {
                log_error("Extra characters after macro name\n\tLine %d: unexpected token: %s\n", lineCount, p_line);
                result->error = EXTRANEOUS_CHARACTERS;
                macros_exit_with_clean(p_macros, p_macroName, p_macroValue, p_newFileName, p_file, p_newFile);
                return result;
            }


            /* Check if the macro is already defined */
            if (ht_get(p_macros, p_macroName) != NULL)
            {
                log_error("Multiple macro definitions\n\tLine %d: Macro %s is already defined\n", lineCount, p_macroName);
                result->error = MULTIPLE_MACRO_DEFINITIONS;
                macros_exit_with_clean(p_macros, p_macroName, p_macroValue, p_newFileName, p_file, p_newFile);
                return result;
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
                p_macroValueGet = ht_get(p_macros, p_line);
                
                if (p_macroValueGet != NULL)
                {
                    fprintf(p_newFile, "%s", p_macroValueGet);
                }
                else
                {
                    log_warning("An invalid macro name was found: %s at line %d\n", p_line, lineCount);
                    fprintf(p_newFile, "%s", line);
                }
            }
            else
            {
                fprintf(p_newFile, "%s", line);
            }
            safe_free(word);
        }
    }

    log_info("Done searching for macros in file %s\n", p_fileName);
    return result;
}

