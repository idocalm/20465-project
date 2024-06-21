#include "program.h"
#include "errors.h"

/* Main function of our assembler */
int main(int argc, char *argv[]) {
    int i = 1; 

    for (; i < argc, i++) {
        sturct macro *macroHead = NULL;

        /* Allocate memory for the macro head */
        macroHead = (struct macro *) malloc(sizeof(struct macro));
        if (macroHead == NULL) {
            printf("[Error]: Could not allocate memory for the macro head in iteration %d \n", i);
            return 1;
        }

        int resultCode = scanMacros(argv[i], macroHead)

        switch (resultCode) {
            case UNABLE_TO_READ_FILE:
                printf("[Error]: Could not open file %s in iteration %d \n", argv[i], i);
                return 1;
            case UNVALID_MACRO_NAME:
                printf("[Error]: Invalid macro name (saved name) in file %s in iteration %d \n", argv[i], i);
                return 1;
        }

        if (resultCode == 0) {
            replaceMacros(argv[i], macroHead);
        }

    }
}