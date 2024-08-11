#include "macros.h"

/**
    * @brief Main function to handle all phases of macro detection and replacement (pre-processor)
    * @param file_name - the name of the file (.am here)
    * @param macros - the list of macros we're going to fill.
    * @return MacroError - the result of the operation (NO_MACRO_ERROR for success or MACRO_ERROR for failure of some sort)
*/
MacroError handle_macros(char *file_name, List *macros) {

    MacroError res = NO_MACRO_ERROR; /* The result of the operation */

    FILE *input_file = open_file(file_name, "r"); /* Open the file with reading perm */

    /* We first extract all the macros to a list, while checking for possible errors */
    if (extract_macros(input_file, macros) != NO_MACRO_ERROR) {
        res = MACRO_ERROR;
    }

    /* Replace macros, check for "label=macro" errors and create .am file */
    if (replace_macros(input_file, file_name, macros, res) != NO_MACRO_ERROR) {
        res = MACRO_ERROR;
    }

    return res;
}

/**
    * @brief Function that looks a up a macro with a name that matches the line
    * @param line - the line from the file to search in 
    * @param macros - the list of macros to search in
*/

void *search_in_macros(char *line, List *macros) {
    Node *current = macros->head;
    while (current != NULL) {
        char *lookup = strstr(line, current->key); /* Check for a match */

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
    * @brief Validate that the argument of a extern instruction is not a macro name.
    * @param line - the line to check
    * @param macros - the list of macros
    * @param type - 0 for .entry, 1 for .extern
    * @param line_num - the line number (for errors )
*/

int validate_label(char *line, int type, List *macros, int line_num) {
    char *label = NULL;
    char *searching = type == 0 ? ".entry" : ".extern";
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
    * @brief Main replacement of macros function. In addition, we analyze labels that appear and check they're not macros. 
    * @param input_file - the file to read from
    * @param file_name - the name of the file (so we can create .as files) 
    * @param macros - the list of macros (found at extract_macros func )
    * @param found_error - a flag to see if an error was found before hand (if so, we dont create the .am file)
    * @return MacroError - the result of the operation (NO_MACRO_ERROR for success or MACRO_ERROR for failure)

*/

MacroError replace_macros(FILE *input_file, char *file_name, List *macros, MacroError found_error) {

    int line_num = 0; /* Used to keep track of the current line in the input file. */
    int inside_macro = 0; /* Used to keep track of macro definitions*/
    
    char line[MAX_LINE_SIZE + 2]; /* Will store the line with fgets */ 
    char *p_line; 
    size_t file_size; /* So we can allocate the right size for the output */ 
    char *output; /* The output string */
    FILE *output_file; 
    int found_error_now = 0; /* Used to check if an error was found in the current line */
    void *result; /* Matched macro that we would replace with the line */

    /* Label def */
    char label[MAX_LINE_SIZE + 1];
    label[MAX_LINE_SIZE] = '\0';

    /* Find the file size */
    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    /* 
        Allocate for output. 
        Note that we do it this way because we dont know yet if we wont run into an error,
        only after we finish going over the lines do we know if .am file should or should not be created! 
    */

    output = (char *) safe_malloc(file_size + 1);
    output[0] = '\0';

    /* Label information (for a possible one/ if it exist s) */ 

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) 
    {   
        line_num++;
        p_line = line;

        /* Skip any leading spaces */
        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n') {
            /* Empty lines */
            continue;
        }

        /* Validate labels are not macros */
        memset(label, 0, MAX_LINE_SIZE + 1);
        is_label_error(p_line, line_num, label, 0);
        if (label[0] != '\0' && search_in_macros(label, macros) != NULL) {
            log_line_error(line_num, line, "Invalid label: the label '%s' is also a macro.", label);
            found_error_now = 1;
            continue;
        }

        /* Validate .entry / .extern are not macros */

        if (strstr(p_line, ".entry") != NULL && !validate_label(p_line, 0, macros, line_num)) {
            found_error_now = 1;
            continue;
        }

        if (strstr(p_line, ".extern") != NULL && !validate_label(p_line, 1, macros, line_num)) {
            found_error_now = 1;
            continue;   
        }
        
        /* We skip a command without copying it to the .am file */
        if (is_comment(p_line))
            continue;

        if (inside_macro && strncmp(p_line, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0) {
            /* Reached the end of a macro definition */
            inside_macro = 0; 
            continue;
        } 

        if (!inside_macro && strncmp(p_line, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0 && isspace(p_line[strlen(MACRO_START_PREFIX)])) {
            /* Reached the start of a macro definition */
            inside_macro = 1;
            continue;
        }

        /* We avoid copying the actual content of any macro into the .am file */
        if (inside_macro) 
            continue;
        
        skip_spaces(&p_line);

        result = search_in_macros(p_line, macros);
        if (result != NULL) /* Add the macro content to the output */
            strcat(output, result);
        else /* Add the line to the output */
            strcat(output, line);
    }

    /* Close the input file */
    close_file(input_file);

    if (found_error_now || found_error) {
        safe_free(output);
        return MACRO_ERROR;
    }

    /* Proceed with .am creation */

    output = (char *) safe_realloc(output, strlen(output) + 1);
    output[strlen(output)] = '\0';

    file_name[strlen(file_name) - 1] = 'm';
    output_file = open_file(file_name, "w");

    /* Write the content to the .am file */
    fprintf(output_file, "%s", output);

    /* Close & free memory */
    close_file(output_file);
    safe_free(output);

    log_success("Replaced macros successfully\n");
    return NO_MACRO_ERROR;

}

/**
    * @brief Extracts all macros from the input file and stores them in the macros list
    * @param input_file - the file to read from
    * @param macros - the list to store the macros in
    * @return MacroError - the result of the operation (NO_MACRO_ERROR for success or MACRO_ERROR for failure)
*/
MacroError extract_macros(FILE *input_file, List *macros) {

    int line_num = 0;  
    char line[MAX_LINE_SIZE + 2];
    char *p_line;

    int found_error = 0;
    char *macro_name = NULL;
    char *macro_content = NULL;
    char *p_search;
    int inside_macro = 0;

    size_t macroContentSize = 0;

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) 
    {
        p_line = line;
        line_num++;

        if (strlen(p_line) > MAX_LINE_SIZE) {
            log_line_error(line_num, line, "Line is too long. (%d chars)", strlen(p_line));
            found_error = 1;
            continue;
        }

        p_search = p_line;
        skip_spaces(&p_search);

        if (!inside_macro && strncmp(p_search, MACRO_START_PREFIX, MACRO_START_PREFIX_LEN) == 0) { /* Search for a possible macro definition */
            /* Check if the next character is a space */
            p_search += MACRO_START_PREFIX_LEN;
            if (!isspace(*p_search)) {
                continue; /* Fake definition */
            }

            /* Allocate extra space for the macro name */
            if (macro_name) {
                safe_free(macro_name);
                macro_name = NULL;
            }
            /* Allocate space for the macro content */
            if (macro_content) 
            {
                safe_free(macro_content);
                macro_content = NULL;
            }

            macro_content = (char *) safe_malloc(1);
            macro_content[0] = '\0';
            macroContentSize = 1; 

            skip_spaces(&p_search);

            /* Copy the macro name */
            macro_name = copy_string_until_space(p_search);

            /* Check if the macro name is valid */
            if (strlen(macro_name) <= 0) {
                log_line_error(line_num, line, "Invalid macro definition: Macro name is missing");
                found_error = 1;
                continue;
            }



            /* Make sure there are no non-space characters after the macro name */
            p_search += strlen(macro_name);
            skip_spaces(&p_search);
            if (*p_search != '\0' && *p_search != '\n') {
                log_line_error(line_num, line, "Extraneous characters appear after the macro name.");
                found_error = 1;
                continue;
            }

            if (is_reserved_word(macro_name)) {
                log_line_error(line_num, line, "Invalid macro definition: Macro name is a reserved word.");
                found_error = 1;
                continue;
            }


            /* Check if the macro is already defined */
            if (list_get(macros, macro_name) != NULL) {
                log_line_error(line_num, line, "Multiple macro definitions in the same file.");
                found_error = 1;
                continue;
            }

            inside_macro = 1;                

            continue;

        } 

        /* Check if the line is the end of the macro */
        if (strncmp(p_search, MACRO_END_PREFIX, MACRO_END_PREFIX_LEN) == 0) {

            p_search += MACRO_END_PREFIX_LEN;
            skip_spaces(&p_search);

            if (*p_search != '\0' && *p_search != '\n') {
                log_line_error(line_num, line, "Invalid macro definition: Extraneous characters after 'endmacr'.");
                found_error = 1;
                continue;
            }

            macro_content[macroContentSize] = '\0';

            list_insert_string(macros, macro_name, macro_content);

            /* Free the memory of the macro */
            safe_free(macro_name);
            safe_free(macro_content);
            macro_name = NULL;
            macro_content = NULL;
            macroContentSize = 0;


            inside_macro = 0; /* We're no longer inside a macro */
            continue;
        } 

        if (inside_macro) {
            /* Add the line to the macro content */
            macro_content = (char *) safe_realloc(macro_content, macroContentSize + strlen(p_line) + 1);
            strcat(macro_content, p_line);
            macroContentSize += strlen(p_line);
        }
    }

    /* Free memory if needed */
    if (macro_name) 
        safe_free(macro_name);
    if (macro_content)
        safe_free(macro_content);

    if (found_error) {
        return MACRO_ERROR; /* Theres an error somewhere */
    }

    return NO_MACRO_ERROR; /* No errors found */
}