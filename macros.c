#include "macros.h"

MacroErrors handle_macros(char *p_fileName, List *p_macros) {

    MacroErrors res; 

    FILE *p_file = open_file(p_fileName, "r");
    res = extract_macros(p_file, p_macros);
    if (res != NO_MACRO_ERROR) {
        return res;
    } 

    replace_macros(p_file, p_fileName, p_macros);



    return NO_MACRO_ERROR;
}


void replace_macros(FILE *p_file, char *p_fileName, List *p_macros) {

    int lineNum = 1;
    int found = 0;
    int insideMacro = 0;
    char line[MAX_LINE_SIZE + 2];
    char *p_line;
    size_t fileSize; 
    struct Node *p_current = p_macros->head;
    char *output;
    FILE *p_amFile;

    fseek(p_file, 0, SEEK_END);
    fileSize = ftell(p_file);
    fseek(p_file, 0, SEEK_SET);

    output = (char *) safe_malloc(fileSize + 1);
    output[0] = '\0';


    while (fgets(line, MAX_LINE_SIZE + 2, p_file) != NULL) 
    {   
        p_line = line;

   
        skip_spaces(&p_line);


        if (is_comment(p_line)) {
            lineNum++;
            continue;
        }

        if (insideMacro && strncmp(p_line, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0) {
            insideMacro = 0; 
            lineNum++;
            continue;
        } 

        if (!insideMacro && strncmp(p_line, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0 && isspace(p_line[strlen(MACRO_START_PREFIX)])) {
            insideMacro = 1;
            lineNum++;
            continue;
        }

        if (insideMacro) {
            lineNum++;
            continue;
        }


        /* Go over the p_macros and search them in the line with strstr */
        found = 0;

        p_current = p_macros->head;
        while (p_current != NULL) {
            skip_spaces(&p_line);
            char *p_macroPos = strstr(p_line, p_current->key);
            if (p_macroPos != NULL) {

                char *p_validate = p_line + strlen(p_current->key);
                /* make sure there's a space right after the macro name */
                if (!isspace(*p_validate) && *p_validate != '\0') {
                    p_current = p_current->next;
                    continue;
                }
    
                /* check there are no letters after the macro name */
            
                char *p_macroEnd = p_validate;
                while (isspace(*p_macroEnd)) {
                    p_macroEnd++;
                }

                if (*p_macroEnd != '\0' && *p_macroEnd != '\n') {
                    log_error("Extra after macro\n");
                    p_current = p_current->next;
                    continue;
                }               

                strcat(output, p_current->data);
                found = 1;
                break;

            }
            p_current = p_current->next;
        }


        if (found == 0) {
            strcat(output, line);
        }

        lineNum++;

    }

    close_file(p_file);

    output = (char *) safe_realloc(output, strlen(output) + 1);
    output[strlen(output)] = '\0';


    p_fileName[strlen(p_fileName) - 1] = 'm';



    p_amFile = open_file(p_fileName, "w");

    fprintf(p_amFile, "%s", output);

    close_file(p_amFile);
    safe_free(output);

    
}

MacroErrors extract_macros(FILE *p_file, List *p_macros) {
    int lineNum = 1;
    char line[MAX_LINE_SIZE + 2];
    char *p_line;

    char *macroName = NULL;
    char *macroContent = NULL;
    int insideMacro = 0;

    size_t macroContentSize = 0;

    while (fgets(line, MAX_LINE_SIZE + 2, p_file) != NULL) 
    {
        p_line = line;
        if (strlen(p_line) > MAX_LINE_SIZE) {
            log_error("Line is too long: \n\tLine %d contains %ld characters while the maximum is %d\n", lineNum, strlen(p_line), MAX_LINE_SIZE); 
            safe_free(macroName);
            safe_free(macroContent);
            return LINE_LENGTH_EXCEEDED;
        }


        if (!insideMacro) {
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
                macroName = (char *) safe_malloc(strlen(p_search) + 1);
                
                skip_spaces(&p_search);

                copy_string_until_space(macroName, p_search);
                macroName = (char *) safe_realloc(macroName, strlen(macroName) + 1); /* Reallocate the exact size */
                macroName[strlen(macroName)] = '\0';


                macroContent = (char *) safe_malloc(1);
                macroContent[0] = '\0';
                macroContentSize = 1; 

                if (strlen(macroName) <= 0) {
                    log_error("Invalid macro definition in line %d\n\t Macro name is missing\n", lineNum);
                    safe_free(macroName);
                    return NO_MACRO_NAME;
                }

                /* Make sure there are no non-space characters after the macro name */
                p_search += strlen(macroName);
                skip_spaces(&p_search);
                if (*p_search != '\0' && *p_search != '\n') {
                    log_error("Invalid macro definition in line %d\n\t Extraneous characters after macro name\n", lineNum);
                    safe_free(macroName);
                    return EXTRANEOUS_CHARACTERS;
                }


                

                if (is_reserved_word(macroName)) {
                    log_error("Invalid macro name in line %d\n\t Macro name %s is a reserved word\n", lineNum, macroName);
                    safe_free(macroName);
                    safe_free(macroContent);
                    return INVALID_MACRO_NAME;
                }

                /* Check if the macro is already defined */
                if (list_get(p_macros, macroName) != NULL) {
                    log_error("Multiple macro globals in line %d\n\t Macro %s is already defined\n", lineNum, macroName);
                    safe_free(macroName);
                    return MULTIPLE_MACRO_DEFINITIONS;
                }

                insideMacro = 1;                
            }


        } else {
            char *p_search = p_line;
            skip_spaces(&p_search);
            /* Check if the line is the end of the macro */
            if (strncmp(p_search, MACRO_END_PREFIX, MACRO_END_PREFIX_LEN) == 0) {

                p_search += MACRO_END_PREFIX_LEN;
                skip_spaces(&p_search);

                if (*p_search != '\0' && *p_search != '\n') {
                    log_error("Invalid macro definition in line %d\n\t Extraneous characters after macro end\n", lineNum);
                    safe_free(macroName);
                    safe_free(macroContent);
                    return EXTRANEOUS_CHARACTERS;
                }


                macroContent = (char *) safe_realloc(macroContent, macroContentSize + 1);
                macroContent[macroContentSize] = '\0';

                list_insert_string(p_macros, macroName, macroContent);

                insideMacro = 0;
                safe_free(macroName);
                safe_free(macroContent);
                macroContentSize = 0;

            } else {
                /* Add the line to the macro content */

                macroContent = (char *) safe_realloc(macroContent, macroContentSize + strlen(p_line) + 1);
                strcat(macroContent, p_line);
                macroContentSize += strlen(p_line);
            }

        }

        lineNum++;
    }

    printf("Finished extracting macros\n");


    return NO_MACRO_ERROR;
}







