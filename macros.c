#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#include "definitions.h"
#include "macros.h"

#include "syntax/helpers.h"
#include "syntax/symbols.h"

void extract_file_content(const char *p_fileName, char **pp_content) {
    FILE *p_file = NULL;
    long length;

    p_file = open_file(p_fileName, "r");

    fseek(p_file, 0, SEEK_END);
    length = ftell(p_file);
    rewind(p_file);

    *pp_content = (char *)safe_malloc(length + 1);

    fread(*pp_content, 1, length, p_file);
    (*pp_content)[length] = '\0';

    close_file(p_file);
}

MacroErrors handle_macros(const char *p_fileName) {
    ht_t *p_macros = ht_create();
    char *fileContent = NULL;
    MacroErrors res; 

    extract_file_content(p_fileName, &fileContent);
    res = extract_macros(fileContent, p_macros);
    if (res != NO_ERROR) {
        return res;
    } 

    char *content = replace_macros(fileContent, p_macros);
   
    /* Write the content to a new file */
    char *newFileName = (char *)safe_malloc(strlen(p_fileName));
    strcpy(newFileName, p_fileName);
    newFileName[strlen(p_fileName) - 1] = 'm';
    FILE *p_file = open_file(newFileName, "w");

    fprintf(p_file, "%s", content);

    close_file(p_file);
    safe_free(content);

    return NO_ERROR;
}


char *replace_macros(const char *fileContent, ht_t *p_macros) {

    char *currentLine = NULL;
    const char *nextLine = NULL;
    int insideMacro = 0;
    char *resultContent = NULL;


    currentLine = (char *) safe_malloc(strlen(fileContent));
    strcpy(currentLine, fileContent);

    /* add a \n at the end of the file */
    currentLine[strlen(fileContent)] = '\n';
   
    resultContent = (char *) safe_malloc(strlen(fileContent) + 1);
    resultContent[0] = '\0';
    

    while (currentLine && ((nextLine = strchr(currentLine, '\n')) != NULL)) {

        size_t lineLength = nextLine - currentLine;
        char *line = (char *) safe_malloc(lineLength + 1);
        strncpy(line, currentLine, lineLength);
        line[lineLength] = '\0';

        if (insideMacro) {
            char *endPtr = line;
            while (*endPtr && isspace(*endPtr)) endPtr++;
            if (strncmp(endPtr, MACRO_END_PREFIX, strlen(MACRO_END_PREFIX)) == 0) {
                insideMacro = 0;
            }
        } else {

            char *startPtr = line;
            while (*startPtr && isspace(*startPtr)) startPtr++;
            if (strncmp(startPtr, MACRO_START_PREFIX, strlen(MACRO_START_PREFIX)) == 0 
                && isspace(startPtr[strlen(MACRO_START_PREFIX)])) {
                insideMacro = 1;
            } else {

                char *linePtr = line;

                while (*linePtr) {
                    char *startPtr = linePtr;
                    while (*startPtr && !isspace(*startPtr)) {
                        startPtr++;
                    }

                    char *word = (char *) safe_malloc(startPtr - linePtr + 1);
                    strncpy(word, linePtr, startPtr - linePtr);
                    word[startPtr - linePtr] = '\0';


                    /* Check if the word is a macro */
                    char *macroContent = ht_get(p_macros, word);

                    

                    if (macroContent) {
                        resultContent = (char *)safe_realloc(resultContent, strlen(resultContent) + 1);
                        resultContent[strlen(resultContent)] = '\n';

                        
                        resultContent = (char *)safe_realloc(resultContent, strlen(resultContent) + strlen(macroContent) + 1);
                        strcat(resultContent, macroContent);
                    } else {

                        resultContent = (char *)safe_realloc(resultContent, strlen(resultContent) + strlen(word) + 1);
                        strcat(resultContent, word);
                    }

                    safe_free(word);

                    /* Skip any spaces between the word and the next word */
                    linePtr = startPtr;

                    while (*linePtr && isspace(*linePtr) && *linePtr != '\n' && *linePtr != EOF) {
                        size_t spaces = 0;
                        char *startSpaces = linePtr;
                        /* Count the number of spaces we'd need to skip */
                        while (*startSpaces && isspace(*startSpaces)) {
                            spaces++;
                            startSpaces++;
                        }

                        resultContent = (char *)safe_realloc(resultContent, strlen(resultContent) + spaces + 1);
                        strncat(resultContent, linePtr, spaces);
                        linePtr += spaces;
                    }
                    
                }

                /* Add a new line to the result */
                resultContent = (char *)safe_realloc(resultContent, strlen(resultContent) + 2);
                strcat(resultContent, "\n");

            }

        }


        safe_free(line);
        currentLine = nextLine + 1;
    }

    /* Remove the last \n */
    resultContent[strlen(resultContent) - 1] = '\0';

    return resultContent;
}


MacroErrors extract_macros(const char *fileContent, ht_t *p_macros) {
    int lineNum = 1;
    const char *currentLine = fileContent;
    const char *nextLine = NULL;
    char *macroName = NULL;
    char *macroContent = NULL;
    int insideMacro = 0;
    size_t macroContentSize = 0;

    while (currentLine) {
        nextLine = strchr(currentLine, '\n');
        if (nextLine) {
            size_t lineLength = nextLine - currentLine;
            char *line = (char *)safe_malloc(lineLength + 1);
            strncpy(line, currentLine, lineLength);
            line[lineLength] = '\0';

            char *linePtr = NULL;

            if (insideMacro) {
                if ((linePtr = strstr(line, MACRO_END_PREFIX)) != NULL) {
                    insideMacro = 0;
                    macroContent = (char *)safe_realloc(macroContent, macroContentSize + lineLength + 2);

                    /* Copy the line until the end of the macro */


                    strncat(macroContent, line, linePtr - line);
                    strcat(macroContent, "\n");
                    macroContentSize += lineLength + 1;
                    
                    ht_set(p_macros, macroName, macroContent);
                    safe_free(macroName);
                    macroName = NULL;
                    macroContent = NULL;
                    macroContentSize = 0;
                } else {
                    macroContent = (char *)safe_realloc(macroContent, macroContentSize + lineLength + 2);

                    
                    strcat(macroContent, line);
                    strcat(macroContent, "\n");
                    macroContentSize += lineLength + 1;
                }
            } else if ((linePtr = strstr(line, MACRO_START_PREFIX)) != NULL) {
                insideMacro = 1;


                macroName = (char *)safe_malloc(lineLength - MACRO_START_PREFIX_LEN);
                linePtr += MACRO_START_PREFIX_LEN; 
                
                /* Skip any spaces between the perfix and the macro name */
                skip_spaces(&linePtr);


                /* Copy the macro name until the first space */
                int i, j;
                for (i = 0, j = 0; i < lineLength; i++) {
                    if (isspace(linePtr[i])) {
                        break;
                    }

                    macroName[j++] = linePtr[i];
                }                                    

                macroName[j] = '\0';

                if (strlen(macroName) <= 0) {
                    log_error("Invalid macro definition in line %d\n\t Macro name is missing\n", lineNum);
                    safe_free(line);
                    safe_free(macroName);
                    safe_free(macroContent);
                    return NO_MACRO_NAME;
                }




                macroName = (char *)safe_realloc(macroName, strlen(macroName));                                           

                
            
                if (ht_get(p_macros, macroName) != NULL) {
                    log_error("Multiple macro definitions in line %d\n\t Macro %s is already defined\n", lineNum, macroName);
                    safe_free(line);
                    safe_free(macroName);
                    safe_free(macroContent);
                    return MULTIPLE_MACRO_DEFINITIONS;
                }

                Operation op = get_operation(macroName);
                Register reg = get_register(macroName);
                Directive dir = get_directive(macroName);

                if (op != UNKNOWN_OPERATION || reg != UNKNOWN_REGISTER || dir != UNKNOWN_DIRECTIVE) {
                    log_error("Invalid macro name in line %d\n\t Macro name %s is a reserved word\n", lineNum, macroName);
                    safe_free(line);
                    safe_free(macroName);
                    safe_free(macroContent);
                    return INVALID_MACRO_NAME;
                }

                macroContent = (char *)safe_malloc(1);
                macroContent[0] = '\0';
                macroContentSize = 0;
            }

            safe_free(line);
            currentLine = nextLine + 1;
            lineNum++;
        } else {
            break;
        }
    }

    return NO_ERROR;
}







