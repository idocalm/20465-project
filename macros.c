#include "macros.h"

MacroResult *search_macros_in_file(const char *fileName) {
    char *line_start = NULL; /* Pointer to the start of the line */
    char line[MAX_LINE_SIZE]; /* Buffer to store the line */
    int is_in_macro = 0; /* Flag to indicate if we are inside a macro */
    char *macroValue = NULL; /* Buffer to store the current macro value */

    FILE *file = open_file(fileName, "r"); 

    MacroResult *result = (*MacroResult)alloc(sizeof(MacroResult));
    result->error = NO_ERROR;

    HashTable *macros = hashtable_init(); /* Create a new hash table to store the macros */
    result->macros = macros;

    while (fgets(line, MAX_LINE_SIZE, file) != NULL) {
        /* Skip any spaces at the beginning of the line */
        skip_spaces(&line_start);

        /* Check if the line starts with the macro start prefix */
        if (strncmp(line_start, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0) {
            /* Skip the macro start prefix */
            line_start += strlen(MACRO_START_PREFIX);

            /* Skip any spaces after the macro start prefix */
            skip_spaces(&line_start);

            /* Get the macro name */
            char *macroName = line_start;

            /* Find the end of the macro name */
            while (*line_start != ' ' && *line_start != '\t' && *line_start != '\n') {
                line_start++;
            }

            /* Make sure there is a space after the macro name */
            if (*line_start != ' ' && *line_start != '\t' || *line_start == '\n') {
                result->error = EXTRANEOUS_CHARACTERS;
                return result;
            }

            /* Null-terminate the macro name */
            *line_start = '\0';

            /* Skip any spaces after the macro name */
            skip_spaces(&line_start);

            /* Check that the name is not an operation or a register */

            if (is_opertion(macroName) || is_register(macroName) || is_directive(macroName)) {
                result->error = INVALID_MACRO_NAME;
                return result;
            }

            /* Add the macro to the hash table */
            hashtable_putstr(macros, macroName);

            is_in_macro = 1;

        } else if (is_in_macro) {

            /* Skip any spaces at the beginning of the line */
            skip_spaces(&line_start);

            /* Check if the line starts with the macro end prefix */
            if (strncmp(line_start, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0) {
                is_in_macro = 0;

                /* Null-terminate the macro value */
                *macroValue = '\0';

                /* Add the macro value to the hash table */
                hashtable_putstr(macros, macroName, macroValue);


            } else {
                /* Add the line to the macro value */
                strcat(macroValue, line);
            }

        }


    }

    fclose(file);

    return result;

}

int replace_macros_in_file(const char *fileName) {
    /* TODO */ 
    return 0;
}