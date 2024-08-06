#include "command_line.h"

void get_operands(char *line, char **operands, int *operandsCount) {
    int i = 0;
    char *operand = NULL;

    /* split the line by ',' */
    while ((operand = strtok(line, ",")) != NULL) {


        if (i == MAX_OPERANDS + 1) {
            return;
        }

        operands[i] = (char *) safe_malloc(strlen(operand) + 1);

        remove_all_spaces(operand);
        strcpy(operands[i], operand);

        operands[i] = (char *) safe_realloc(operands[i], strlen(operands[i]) + 1);
        i++;
        line = NULL; 
    }
    *operandsCount = i;
}

machine_word *build_single_word(char *source, char *dest, int *ic, int line_num) {

    int found_error = 0;
    Register source_reg, dest_reg;
    machine_word *word; 

    if (*source == '*') {
        source++;
    }

    if (*dest == '*') {
        dest++;
    }

    source_reg = get_register(source);
    dest_reg = get_register(dest);
    word = (machine_word *) safe_malloc(sizeof(machine_word));
    word->ic = *ic;


    if (source_reg == UNKNOWN_REGISTER) {
        log_error("Invalid register in line %d\n\tOperand: %s\n", line_num, source);
        found_error = 1;
    }

    if (dest_reg == UNKNOWN_REGISTER) {
        log_error("Invalid register in line %d\n\tOperand: %s\n", line_num, dest);
        found_error = 1;
    }

    if (found_error) {
        return NULL;
    }

    word->data |= (1 << 2); 
    word->data |= (source_reg << 6);
    word->data |= (dest_reg << 3);

    return word;
}

machine_word *build_additional_word(AddressMode mode, int is_dest, char *operand, int *ic, int line_num) {

    machine_word *word = (machine_word *) safe_malloc(sizeof(machine_word));
    word->ic = *ic;


    if (mode == IMMEDIATE) {
        int value = is_integer(operand + 1);

        if (value == NON_VALID_INTEGER) {
            log_error("Invalid immediate value in line %d\n\tOperand: %s\n", line_num, operand);
            return NULL;
        }

        word->data |= (1 << 2);
        word->data |= (value << 3);
    } else if (mode == REGISTER) {
        Register reg = get_register(operand);

        if (reg == UNKNOWN_REGISTER) {
            log_error("Invalid register in line %d\n\tOperand: %s\n", line_num, operand);
            return NULL;
        }

        if (is_dest) {
            word->data |= (reg << 3);
        } else {
            word->data |= (reg << 6);
        }

        word->data |= (1 << 2);

    } else if (mode == RELATIVE) {

        Register reg = get_register(operand + 1);

        if (reg == UNKNOWN_REGISTER) {
            log_error("Invalid register in line %d\n\tOperand: %s\n", line_num, operand);
            return NULL;
        }


        word->data |= (1 << 2);

        if (is_dest) {
            word->data |= (reg << 3);
        } else {
            word->data |= (reg << 6);
        }

   }

    return word;

}

machine_word *build_first_word(Operation op, AddressMode source, AddressMode dest, int *ic, int line_num) {
    machine_word *word = (machine_word *) safe_malloc(sizeof(machine_word));

    word->data = 0;

    word->data |= (1 << 2);
    word->data |= (op << 11);
    word->ic = *ic;
    if (dest != UNKNOWN_ADDRESS) {
        word->data |= (1 << (3 + dest));
    }
    if (source != UNKNOWN_ADDRESS) {
        word->data |= (1 << (7 + source));
    }


    return word;

}   

void free_operands(char **operands, int operandsCount) {
    int i;
    for (i = 0; i < operandsCount; i++) {
        safe_free(operands[i]);
    }
    safe_free(operands);
}

int handle_directive_line(char *line, int line_num, int *ic, Labels *labels, machine_word **code_image) {
    char *operationName = NULL;
    char **operands = safe_malloc(MAX_OPERANDS * sizeof(char *));
    AddressMode dest, source; 
    int i = 0; 

    int is_source_reg, is_dest_reg;

    Operation op; 
    OperationGroup op_group;
    int operandsCount = 0;

    machine_word *first_word = NULL;
    machine_word *second_word = NULL;
    machine_word *third_word = NULL;
    char label[MAX_LABEL_SIZE + 1];
    label[MAX_LABEL_SIZE] = '\0';
    int found_error = 0;

    /* Replace the last character with a null terminator */

    skip_spaces(&line);

    /* #1 - Extract the label, if exists. */
    if (is_label_error(line, line_num, label, 1)) {
        found_error = 1;
        printf("Returning at: %d\n", line_num);
        return 1; /* Maybe this shouldn't be a return */
    }


    if (label[0] != '\0') {
        LabelEntry *entry = labels_get(labels, label);
        if (entry != NULL && entry->type == CODE_LABEL) {
            log_error("Label already defined in line %d\n\tLabel: %s\n", line_num, label);
            found_error = 1;
        }
        
        line += strlen(label) + 1;
        labels_insert(labels, label, *ic, CODE_LABEL);

    }


    skip_spaces(&line);

    /*
        #2 - Extract the operation name.
    */

    operationName = (char *) safe_malloc(MAX_LINE_SIZE);
    copy_string_until_space(operationName, line);

    operationName = (char *) safe_realloc(operationName, strlen(operationName) + 1);

    op = get_operation(operationName);

    if (op == UNKNOWN_OPERATION) {
        log_error("Invalid operation name in line %d\n\tOperation name: %s at: ...%s\n", line_num, operationName, line);
        safe_free(operationName);
        free_operands(operands, operandsCount);
        return 0;
    }

    op_group = get_operation_group(op);
    line += strlen(operationName);

    skip_spaces(&line);

    
    /*
        #3 - Extract the operands.
    */

    get_operands(line, operands, &operandsCount);

    if (operands == NULL) { 
        log_error("Invalid number of operands in line %d\n\tExpected: 0-2, got: more then 2\n", line_num);
        found_error = 1;
    }

    if (operandsCount != (int) op_group) {
        log_error("Invalid number of operands in line %d\n\tExpected: %d, got: %d\n", line_num, op_group, operandsCount);
        found_error = 1;
    }

    /* Validate operands are not made by more then 1 word. For example: "r1 r2" is invalid. */
    for (i = 0; i < operandsCount; i++) {
        if (strchr(operands[i], ' ') != NULL) {
            log_error("Invalid operand in line %d\n\tOperand: %s at: ...%s \t(missing ',')\n", line_num, operands[i], line);
            found_error = 1;
        }
    }


    if (operandsCount == 1) {
        dest = find_addressing_mode(operands[0]);
        source = UNKNOWN_ADDRESS;
    } else if (operandsCount == 2) {
        source = find_addressing_mode(operands[0]);
        dest = find_addressing_mode(operands[1]);
    } else {
        dest = UNKNOWN_ADDRESS;
        source = UNKNOWN_ADDRESS;
    } 

    /* Validate operands by their addressmode */

    if (!valid_command_with_operands(op, dest, source)) {
        log_error("Invalid operands in line %d.\n\t The Operation: %d does not support addressing modes of source/dest operands\n\tDest: %d, Source: %d\n", line_num, op, dest, source);
        found_error = 1;
    }




    /* Build the instruction */
    first_word = build_first_word(op, source, dest, ic, line_num);
    code_image[*ic - INITIAL_IC_VALUE] = first_word;
    (*ic)++;

    is_source_reg = source == REGISTER || source == RELATIVE;
    is_dest_reg = dest == REGISTER || dest == RELATIVE;

    if (is_source_reg && is_dest_reg) {

        second_word = build_single_word(operands[0], operands[1], ic, line_num);
        code_image[*ic - INITIAL_IC_VALUE] = second_word;
        (*ic)++;
    } else if (operandsCount == 1) {
        second_word = build_additional_word(dest, 1, operands[0], ic, line_num);
        code_image[*ic - INITIAL_IC_VALUE] = second_word;
        (*ic)++;
    } else if (operandsCount == 2) {
        second_word = build_additional_word(source, 0, operands[0], ic, line_num);
        code_image[*ic - INITIAL_IC_VALUE] = second_word;
        (*ic)++;
        third_word = build_additional_word(dest, 1, operands[1], ic, line_num);
        code_image[*ic - INITIAL_IC_VALUE] = third_word;
        (*ic)++;
    } 

    safe_free(operationName);
    free_operands(operands, operandsCount);

    if (found_error) 
        return 0;
    

    return 1;

}
