#include "commands.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "helpers.h"
#include "symbols.h"

char *is_label(char *p) {
    
    char *p_label = (char *)safe_malloc(MAX_LINE_SIZE);
    char *p_labelStart = p;
    char *p_labelEnd = p;
    int labelLength = 0;

    while (*p_labelEnd != ':') {
        if (*p_labelEnd == '\0') {
            safe_free(p_label);
            return NULL;
        }
        p_labelEnd++;
    }

    labelLength = p_labelEnd - p_labelStart;
    strncpy(p_label, p_labelStart, labelLength);
    p_label[labelLength] = '\0';

    return p_label;

}



void **get_operands(char *line, char **operands, int *operandsCount) {
    int i = 0;
    char *operand = NULL;

    /* split the line by ',' */
    while ((operand = strtok(line, ",")) != NULL) {

        if (i == MAX_OPERANDS + 1) {
            return NULL;

        }

        operands[i] = (char *) safe_malloc(strlen(operand));
        remove_all_spaces(operand);
        strcpy(operands[i], operand);
        operands[i] = (char *) safe_realloc(operands[i], strlen(operands[i]));
        i++;
        line = NULL; 
    }
    *operandsCount = i;
}


OperationGroup get_operation_group(Operation op) {
    if (op == MOV || op == CMP || op == ADD || op == SUB || op == LEA) {
        return TWO_OPERANDS;
    } else if (op == CLR || op == NOT || op == INC || op == DEC || op == JMP ||
         op == BNE || op == RED || op == PRN || op == JSR 
    ) {
        return SINGLE_OPERAND;
    } else {
        return NO_OPERANDS;
    }
}


void handle_directive_line(char *line, int lineNum, ht_t *p_macros) {


    char *label = NULL;
    char *operationName = NULL;
    char **operands = safe_malloc(MAX_OPERANDS * sizeof(char *));
    int i = 0; 
    
    Operation op; 
    OperationGroup opGroup;
    int operandsCount = 0;

    /* replace the last character with a null terminator */
    line[strlen(line) - 1] = '\0';

    /* #1 - Extract the label, if exists. */

    label = is_label(line);
    if (label != NULL) {

        /* check that there are no spaces between the label and the ':' and no more then one word */
        char *space = strchr(label, ' ');
        if (space != NULL) {
            if (space - label + 1 < strlen(label)) {
                log_error("Invalid label in line %d\n\tLabel: %s contains spaces\n", lineNum, label);
                safe_free(label);
                return;
            }
            log_error("Invalid label in line %d\n\tLabel: %s, ':' does not appear close to definition\n", lineNum, label);
            safe_free(label);
            return;
        }

        /* check that the label is not a reserved word */
        if (get_operation(label) != UNKNOWN_OPERATION || get_register(label) != UNKNOWN_REGISTER || get_directive(label) != UNKNOWN_DIRECTIVE) {
            log_error("Invalid label in line %d\n\tLabel: %s is a reserved word\n", lineNum, label);
            safe_free(label);
            return;
        }


        
        /* 
            TODO: check that the label is not a macro, this caused an error.
        */ 

        printf("Label: %s\n", label);
        
        /* check that the label is not too long */
        if (strlen(label) > MAX_LABEL_SIZE) {
            log_error("Label is too long in line %d\n\tLabel: %s. \n\tThe maximum length is %d while the label is %ld\n", lineNum, label, MAX_LABEL_SIZE, strlen(label));
            safe_free(label);
            return;
        }
        line += strlen(label) + 1;
    } 

    skip_spaces(&line);

    /*
        #2 - Extract the operation name.
    */

    operationName = (char *)safe_malloc(MAX_LINE_SIZE);
    copy_string_until_space(operationName, line);
    operationName = (char *)safe_realloc(operationName, strlen(operationName) + 1);

    op = get_operation(operationName);

    if (op == UNKNOWN_OPERATION) {
        log_error("Invalid operation name in line %d\n\tOperation name: %s at: ...%s\n", lineNum, operationName, line);
        safe_free(operationName);
        return;
    }

    opGroup = get_operation_group(op);
    line += strlen(operationName);
    skip_spaces(&line);


    /*
        #3 - Extract the operands.
    */

    get_operands(line, operands, &operandsCount);

    if (operands == NULL) {
        log_error("Invalid number of operands in line %d\n\tExpected: 0-2, got: more then 2\n", lineNum);
        safe_free(operationName);
        return;
    }

    if (operandsCount != (int) opGroup) {
        log_error("Invalid number of operands in line %d\n\tExpected: %d, got: %d\n", lineNum, opGroup, operandsCount);
        safe_free(operationName);
        return;
    }

    /* Validate operands are not made by more then 1 word. For example: "r1 r2" is invalid. */
    for (i = 0; i < operandsCount; i++) {
        if (strchr(operands[i], ' ') != NULL) {
            log_error("Invalid operand in line %d\n\tOperand: %s at: ...%s \t(missing ',')\n", lineNum, operands[i], line);
            safe_free(operationName);
            return;
        }
    }





}

void handle_instruction_line(char *line) {
    /*printf("Handling instruction line: %s\n", line);*/ 
}

