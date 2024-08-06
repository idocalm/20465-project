#include "macros.h"


MacroErrors handle_macros(char *file_name, List *macros) {

    MacroErrors res; 

    FILE *input_file = open_file(file_name, "r");

    /* We first extract all the macros to a list, while checking for possible errors */
    res = extract_macros(input_file, macros);
    if (res != NO_MACRO_ERROR) {
        return res;
    } 

    /* Replace macros and create .am file */
    replace_macros(input_file, file_name, macros);
    return NO_MACRO_ERROR;
}

void *search_macros(char *line, List *macros) {
    Node *current = macros->head;
    while (current != NULL) {
        char *lookup = strstr(line, current->key);
        if (lookup != NULL) {
            lookup += strlen(current->key);

            while (isspace(*lookup)) {
                lookup++;
            }

            if (*lookup == '\0' || *lookup == '\n') {
                return current->data;
            }


        }
        current = current->next;
    }

    return NULL;
}

void replace_macros(FILE *input_file, char *file_name, List *macros) {

    int line_num = 1; /* Used to keep track of the current line in the input file. */
    int inside_macro = 0; /* Used to keep track of macro definitions*/
    
    char line[MAX_LINE_SIZE + 2]; /* Will store the line with fgets */ 
    char *p_line;
    size_t file_size; 
    char *output;
    FILE *output_file;
    void *result;

    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    output = (char *) safe_malloc(file_size + 1);
    output[0] = '\0';


    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) 
    {   
        p_line = line;

        skip_spaces(&p_line);

        if (is_comment(p_line)) {
            /* We skip a command without copying it to the .am file */
            line_num++;
            continue;
        }

        if (inside_macro && strncmp(p_line, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0) {
            /* Reached the end of a macro definition */
            inside_macro = 0; 
            line_num++;
            continue;
        } 

        if (!inside_macro && strncmp(p_line, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0 && isspace(p_line[strlen(MACRO_START_PREFIX)])) {
            /* Reached the start of a macro definition */
            inside_macro = 1;
            line_num++;
            continue;
        }

        if (inside_macro) {
            /* We avoid copying the actual content of any macro into the .am file */
            line_num++;
            continue;
        }


        skip_spaces(&p_line);
        result = search_macros(p_line, macros);
        if (result != NULL) {
            strcat(output, result);
        } else {
            strcat(output, line);
        }

        line_num++;

    }

    close_file(input_file);
    output = (char *) safe_realloc(output, strlen(output) + 1);
    output[strlen(output)] = '\0';

    file_name[strlen(file_name) - 1] = 'm';
    output_file = open_file(file_name, "w");

    fprintf(output_file, "%s", output);

    close_file(output_file);
    safe_free(output);

    log_success("Replaced macros successfully\n");

}

MacroErrors extract_macros(FILE *input_file, List *macros) {

    int line_num = 1; 
    char line[MAX_LINE_SIZE + 2];
    char *p_line;

    int found_error = 0;
    char *macro_name = NULL;
    char *macro_content = NULL;
    int inside_macro = 0;

    size_t macroContentSize = 0;

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) 
    {
        p_line = line;

        if (strlen(p_line) > MAX_LINE_SIZE) {
            log_error("Line is too long: \n\tLine %d contains %ld characters while the maximum is %d\n", line_num, strlen(p_line), MAX_LINE_SIZE); 
            found_error = 1;
            continue;
        }


        if (!inside_macro) {

            /* Search for a possible macro definition */
            char *p_search = p_line;
            skip_spaces(&p_search);

            if (strncmp(p_search, MACRO_START_PREFIX, MACRO_START_PREFIX_LEN) == 0) {
                /* Check if the next character is a space */
                p_search += MACRO_START_PREFIX_LEN;
                if (!isspace(*p_search)) {
                    continue; /* Fake definition */
                }

                /* Allocate extra space for the macro name */
                if (macro_name) 
                    safe_free(macro_name);
                macro_name = (char *) safe_malloc(strlen(p_search) + 1);
                
                skip_spaces(&p_search);

                copy_string_until_space(macro_name, p_search);
                macro_name = (char *) safe_realloc(macro_name, strlen(macro_name) + 1); /* Reallocate the exact size */
                macro_name[strlen(macro_name)] = '\0';

                if (macro_content) 
                    safe_free(macro_content);
                macro_content = (char *) safe_malloc(1);
                macro_content[0] = '\0';
                macroContentSize = 1; 

                if (strlen(macro_name) <= 0) {
                    log_error("Invalid macro definition in line %d\n\t Macro name is missing\n", line_num);
                    found_error = 1;
                    continue;
                }

                /* Make sure there are no non-space characters after the macro name */
                p_search += strlen(macro_name);
                skip_spaces(&p_search);
                if (*p_search != '\0' && *p_search != '\n') {
                    log_error("Invalid macro definition in line %d\n\t Extraneous characters after macro name\n", line_num);
                    found_error = 1;
                    continue;
                }



                if (is_reserved_word(macro_name)) {
                    log_error("Invalid macro name in line %d\n\t Macro name %s is a reserved word\n", line_num, macro_name);
                    found_error = 1;
                    continue;
                }

                /* Check if the macro is already defined */
                if (list_get(macros, macro_name) != NULL) {
                    log_error("Multiple macro globals in line %d\n\t Macro %s is already defined\n", line_num, macro_name);
                    found_error = 1;
                    continue;
                }

                inside_macro = 1;                
            }


        } else {
            char *p_search = p_line;
            skip_spaces(&p_search);
            /* Check if the line is the end of the macro */
            if (strncmp(p_search, MACRO_END_PREFIX, MACRO_END_PREFIX_LEN) == 0) {

                p_search += MACRO_END_PREFIX_LEN;
                skip_spaces(&p_search);

                if (*p_search != '\0' && *p_search != '\n') {
                    log_error("Invalid macro definition in line %d\n\t Extraneous characters after macro end\n", line_num);
                    found_error = 1;
                    continue;
                }

                macro_content = (char *) safe_realloc(macro_content, macroContentSize + 1);
                macro_content[macroContentSize] = '\0';

                list_insert_string(macros, macro_name, macro_content);

                safe_free(macro_name);
                safe_free(macro_content);

                inside_macro = 0;
                macroContentSize = 0;

            } else {
                /* Add the line to the macro content */

                macro_content = (char *) safe_realloc(macro_content, macroContentSize + strlen(p_line) + 1);
                strcat(macro_content, p_line);
                macroContentSize += strlen(p_line);
            }

        }

        line_num++;
    }

    if (found_error) {
        if (macro_name) 
            safe_free(macro_name);
        if (macro_content)
            safe_free(macro_content);
        return MACRO_ERROR;
    }

    return NO_MACRO_ERROR;
}