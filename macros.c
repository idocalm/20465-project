#include "macros.h"

/* 
    This function checks if a string is a macro definition 
    if it is, it returns a pointer to a macro struct, otherwise it returns NULL

    This function does NOT fill the macro with the content, it only fills the name field. 

*/


struct macro *isMacroDefinition(char *line) {

    /* Spaces or tabs at the beginning of the line are ignored */
    int i = 0;
    skipSpaces(line, &i);

    char *macroName = NULL;
    struct macro *macro = NULL;


    if (strncmp(line + i, MACRO_START_KEYWORD, strlen(MACRO_START_KEYWORD)) != 0) {
        return NULL; 
    }


    /* Found a macro definition, skip over it to find the name */ 
    i += strlen(MACRO_START_KEYWORD) + 1; 

    /* Skip over spaces and tabs */
    skipSpaces(line, &i);

    macroName = line + i;


    while (macroName[i] != ' ' && macroName[i] != '\t' && macroName[i] != '\0') {
        i++;
    }


    macroName[i] = '\0';

    /* Check if there are extraneous characters after the macro name */

    skipSpaces(line, &i);

    if (line[i] != '\0') {
        return EXTRANEOUS_MACRO_CHARS;
    }

    if (validateMacroName(macroName) != 0) {
        return UNVALID_MACRO_NAME;
    }

    /* Allocate memory for the macro */

    macro = (struct macro *) malloc(sizeof(struct macro));

    if (macro == NULL) {
        printf("[Error]: Could not allocate memory for the macro %s \n", macroName);
        exit(1);
    }

    /* Assign values to the macro struct */
    strcpy(macro->name, macroName);
    strcpy(macro->value, "\0");
    macro->next = NULL;
    

    return macro;

}

/* 
    This function is used to determine the macro value after isMacroDefinition has been called successfully.
    It fills the value field of the macro struct with the macro value.
*/

void fillMacroValue(FILE *file, struct macro *macro) {

    char line[MAX_LINE_LENGTH];
    char macroValue[MAX_LINE_LENGTH];
    int i = 0;
    int readFlag = 1; 

    while (readFlag) {
        
        fgets(line, MAX_LINE_LENGTH, file);

        if (strncmp(line, MACRO_END_KEYWORD, strlen(MACRO_END_KEYWORD)) == 0) {
            readFlag = 0;
        }

        strncat(macroValue, line, MAX_LINE_LENGTH);


        i += strlen(line);
    }

    macroValue[i] = '\0';

    strcpy(macro->value, macroValue);
}


int scanMacros(char *fileName, struct macro *macro) {
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
        return UNABLE_TO_OPEN_FILE;
    }

    char line[MAX_LINE_LENGTH];
    struct macro *currentMacro = NULL;

    while (fgets(line, MAX_LINE_LENGTH, file)) {

        currentMacro = NULL;

        if ((currentMacro = isMacroDefinition(line)) != NULL) {

            /* Fill the macro value */
            fillMacroValue(file, currentMacro);


            /* Add the macro to the list */
            macro->next = currentMacro;
            macro = currentMacro;

        }        

    }

    /* Close the file for now */

    fclose(file);

    return 0; 

}

void replaceMacros(char *fileName, struct macro *macroHead) {

    FILE *originalFile = fopen(fileName, "r");
    FILE *newFile = NULL;
    char line[MAX_LINE_LENGTH];
    char newFile[MAX_LINE_LENGTH];

    if (originalFile == NULL) {
        printf("[Error]: Could not open file %s \n", fileName);
        exit(1);
    }

    strcpy(newFile, fileName);
    strcat(newFile, ".am", 3);

    newFile = fopen(newFile, "w");


    if (newFile == NULL) {
        printf("[Error]: Could not open file %s \n", newFile);
        exit(1);
    }

    while (fgets(line, MAX_LINE_LENGTH, originalFile)) {
        struct macro *currentMacro = macroHead;

        while (currentMacro != NULL) {
            if (strncmp(line, currentMacro->name, strlen(currentMacro->name)) == 0) {
                fputs(currentMacro->value, newFile);
            }
            currentMacro = currentMacro->next;
        }

        fputs(line, newFile);
    }

    fclose(originalFile);
    fclose(newFile);

    
}    

