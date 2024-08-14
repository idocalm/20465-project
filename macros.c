#include "macros.h"

/** 
    * @brief Handles macro replacement
    * @param file_name - the name of the file
    * @return a MacroError

    Note: according to the forum and booklet it's safe to assume that a macro definition appears before the first use of it, 
    it's also ok to assume that a label using a macro name (if such exists), will appear only after the macro definition.

    In any other case we would have to work with 2 passes on the file, and who would want that

*/
MacroError handle_macros(char *file_name) {

    FILE *input_file = open_file(file_name, "r"); /* Open the file with reading perm */
    FILE *output_file = NULL; /* The output file */
    /* Create the output file name */
    char output_file_name[MAX_LINE_SIZE + 1];
    strcpy(output_file_name, file_name);
    output_file_name[strlen(file_name) - 1] = 'm';

    /* Open the output file */
    output_file = open_file(output_file_name, "w");
    /* Replace macros, check for "label=macro" errors and create .am file */
    return replace_macros(input_file, output_file, output_file_name);
}

/**
    * @brief Searches for a macro in a given line
    * @param str - the string to search for 
    * @param macros - list of macros
    * @return the macro if found, NULL otherwise
*/

void *search_in_macros(char *str, List *macros) {
    Node *current = macros->head;
    while (current != NULL) {
        char *lookup = strstr(str, current->key); /* Check for a match */

        if (lookup != NULL) {
            lookup += strlen(current->key);

            /* Make sure there are only spaces from now on */
            while (isspace(*lookup)) { 
                lookup++;
            }

            if (*lookup == '\0' || *lookup == '\n') {
                return current->data; /* Found a macro */
            }

        }
        current = current->next;
    }

    return NULL; /* No macro found */
}

/**
    * @brief Validates that a label in a .entry / .extern command is not also a macro. 
    * @param line - the line to validate
    * @param type - 0 for .entry, 1 for .extern
    * @param macros - list of macros
    * @param line_num - the line number
    * @return 1 if the label is valid, 0 otherwise
*/
int validate_label(char *line, int type, List *macros, int line_num) {
    char *label = NULL;
    char *searching = type == 0 ? ".entry" : ".extern"; /* The command we're looking for */
    char *look_for_label = strstr(line, searching) + strlen(searching);
    skip_spaces(&look_for_label);

    label = copy_string_until_space(look_for_label); 
    if (label[0] != '\0' && search_in_macros(label, macros) != NULL) {
        log_line_error(line_num, line, "Invalid label in .extern command: the label '%s' is also a macro. ", label);
        safe_free(label);
        return 0;
    }

    safe_free(label);
    return 1;
}

/**
    * @brief Detects & replaces macros in a given file
    * @param input_file - the input file
    * @param output_file - the output file
    * @param output_file_name - the name of the output file 
    * @return a MacroError

    Note: this function will also DELETE the .am file if an error is found so we won't end up with unwanted files
*/

MacroError replace_macros(FILE *input_file, FILE *output_file, char *output_file_name) {
    int line_num = 0; /* Used to keep track of the current line in the input file. */
    int inside_macro = 0; /* Used to keep track of macro definitions*/
    char line[MAX_LINE_SIZE + 2]; /* Will store the line with fgets */
    char *p_line;

    List *macros = list_create(); /* List of macros */

    char label[MAX_LINE_SIZE + 1]; /* to store the label */
    int found_error = 0; /* Did we find an error? */
    int start_match, end_match; /* Used to determine if we're in a macro definition */

    char *current_name = NULL; /* Current macro name*/
    char *current_content = NULL;  /* Current macro content */
    int current_content_size = 0; /* Current macro content size */
    char *result = NULL; /* The result of the search in the macros */

    label[MAX_LINE_SIZE] = '\0'; 

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) {
        line_num++;
        p_line = line;

        /* Check if the line is too long */
        if (strlen(line) > MAX_LINE_SIZE) {
            log_line_error(line_num, line, "Line is too long. (%d chars)", strlen(line));
            found_error = 1;
        }
    
        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n' || *p_line == COMMENT_PREFIX) /* Empty or comment lines */
            continue;

        /* Find label */

        memset(label, 0, MAX_LINE_SIZE + 1);
        is_label_error(p_line, line_num, label, 0);
        /* Check if the label is a macro */
        if (label[0] != '\0' && search_in_macros(label, macros) != NULL) { /* The is_label_error function sets the first char to \0 if somethings wrong with the label */
            log_line_error(line_num, line, "Invalid label: the label '%s' is also a macro.", label);
            found_error = 1;
            continue;
        }

        /* Validate .entry / .extern are not macros */

        if (strstr(p_line, ".entry") != NULL && !validate_label(p_line, 0, macros, line_num)) {
            found_error = 1;
            continue;
        }

        if (strstr(p_line, ".extern") != NULL && !validate_label(p_line, 1, macros, line_num)) {
            found_error = 1;
            continue;   
        }

        /* Start match: is this a new macro? */
        start_match =  strncmp(p_line, MACRO_START_PREFIX, MACRO_START_PREFIX_LEN) == 0;
        /* End match: is this the end of a macro? */
        end_match = strncmp(p_line, MACRO_END_PREFIX, MACRO_END_PREFIX_LEN) == 0;

        if (!inside_macro && start_match && isspace(*(p_line + MACRO_START_PREFIX_LEN))) { /* Search for a possible macro definition */
            char *validator = p_line + MACRO_START_PREFIX_LEN;

            skip_spaces(&validator);
            current_name = copy_string_until_space(validator);

            /* Check if the macro name is valid */
            if (strlen(current_name) <= 0) {
                log_line_error(line_num, line, "Invalid macro definition: Macro name is missing");
                safe_free(current_name);
                found_error = 1;
                continue;
            }

            /* Make sure there are no non-space characters after the macro name */
            validator += strlen(current_name);
            skip_spaces(&validator);

            /* Check that there are no extra chars over the first word */
            if (*validator != '\0' && *validator != '\n') {
                log_line_error(line_num, line, "Extraneous characters appear after the macro name.");
                safe_free(current_name);
                found_error = 1;
                continue;
            }

            /* Check if the macro name is a reserved word */
            if (is_reserved_word(current_name)) {
                log_line_error(line_num, line, "Invalid macro definition: Macro name is a reserved word.");
                safe_free(current_name);
                found_error = 1;
                continue;
            }

            /* Check if the macro is already defined */
            if (list_get(macros, current_name) != NULL) {
                log_line_error(line_num, line, "Multiple macro definitions in the same file.");
                safe_free(current_name);
                found_error = 1;
                continue;
            }

            inside_macro = 1;
            current_content = (char *) safe_malloc(1);
            current_content[0] = '\0';
            current_content_size = 0;
        } else if (inside_macro && end_match) {

            char *validator = p_line + MACRO_END_PREFIX_LEN;
            if (!isspace(*validator)) {
                continue; /* Fake ending (but there will be one later!) */
            }

            inside_macro = 0;
            skip_spaces(&validator);

            /* Check if there are any extra chars after the end of the macro */
            if (*validator != '\0' && *validator != '\n') {
                log_line_error(line_num, line, "Invalid macro definition: Extraneous characters after 'endmacr'.");
                safe_free(current_content);
                safe_free(current_name);
                found_error = 1;
                continue;
            }

            /* Add the macro to the list */

            current_content[strlen(current_content)] = '\0';
            list_insert_string(macros, current_name, current_content);
            safe_free(current_content);
            safe_free(current_name);
        } else if (inside_macro) {
            /* Add the line to the macro content */
            current_content_size += strlen(line) + 1;   
            current_content = (char *) safe_realloc(current_content, current_content_size);
            strcat(current_content, line);
        } else {

            /* Search for a macro in the line, if so - replace it with the content */
            char search[MAX_LINE_SIZE + 1];
            strcpy(search, line);
            remove_all_spaces(search);

            result = search_in_macros(search, macros);

            if (strlen(search) > 0) 
                strcat(search, "\n");

            /* Add to the file */
            fprintf(output_file, "%s", result != NULL ? (char *) result : search);
        }

    }

    /* Free memory and close files */

    fclose(input_file);
    list_free(macros);
    fclose(output_file);

    if (found_error) {
        /* Remove the output file */
        remove(output_file_name);
        return MACRO_ERROR; /* Found an error */
    }

    return NO_MACRO_ERROR; /* No errors */ 
}
