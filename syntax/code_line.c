#include "code_line.h"

machine_word *build_single_word(char *source, char *dest, int *ic, int line_num) {

    int found_error = 0;
    Register source_reg, dest_reg;
    machine_word *word; 

    if (*source == '*') { /* Skip the '*' */
        source++; 
    }

    if (*dest == '*') { /* Skip the '*' */
        dest++; 
    }

    source_reg = get_register(source);
    dest_reg = get_register(dest);
    word = (machine_word *) safe_malloc(sizeof(machine_word));

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

    } else if (mode == POINTER) {

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

    word->data = 0; /* First we reset the word */

    word->data |= (1 << 2); /* Turning 'A' on in each first word */
    word->data |= (op << 11);

    if (dest != UNKNOWN_ADDRESS) {
        word->data |= (1 << (3 + dest)); 
    }
    if (source != UNKNOWN_ADDRESS) {
        word->data |= (1 << (7 + source));
    }

    return word;
}   

void add_to_code_image(machine_word **code_image, machine_word *word, int *ic, int *found_error) {

    /* Check we're not writing pass memory */
    if (*ic - INITIAL_IC_VALUE >= ASSEMBLER_MAX_CAPACITY) {
        log_error("Code image is full, can't add more words\n");
        *found_error = 1;
        return;
    }

    code_image[*ic - INITIAL_IC_VALUE] = word;
    (*ic)++;
}


int handle_code_line(char *line, int line_num, int *ic, Labels *labels, machine_word **code_image) {
    char *operationName = NULL;
    char **operands = safe_malloc(MAX_OPERANDS * sizeof(char *));
    AddressMode dest = UNKNOWN_ADDRESS;
    AddressMode source = UNKNOWN_ADDRESS;
    int i = 0; 
    int is_source_reg, is_dest_reg;
    Operation op; 
    OperationGroup op_group;
    int operands_count = 0;

    machine_word *first_word = NULL;
    machine_word *second_word = NULL;
    machine_word *third_word = NULL;
    int found_error = 0;

    char label[MAX_LABEL_SIZE + 1];
    label[MAX_LABEL_SIZE] = '\0';

    skip_spaces(&line);

    if (is_label_error(line, line_num, label, 1)) {
        found_error = 1;
        return 1; /* Shouldn't return here. */
    }

    if (label[0] != '\0') {        
        LabelEntry *entry = labels_get_any(labels, label);
        if (entry != NULL) {
            log_error("Label already defined in line %d\n\tLabel: %s\n", line_num, label);
            found_error = 1;
        }

        line += strlen(label) + 1;
        labels_insert(labels, label, *ic, CODE_LABEL);
    }

    skip_spaces(&line);

    operationName = (char *) safe_malloc(MAX_LINE_SIZE);
    copy_string_until_space(operationName, line);
    operationName = (char *) safe_realloc(operationName, strlen(operationName) + 1);

    op = get_operation(operationName);

    if (op == UNKNOWN_OPERATION) {
        log_error("Invalid operation name in line %d\n\tOperation name: %s at: ...%s\n", line_num, operationName, line);
        safe_free(operationName);
        free_operands(operands, operands_count);
        return 0;
    }

    op_group = get_operation_group(op);
    line += strlen(operationName);

    skip_spaces(&line);
    
    /*
        #3 - Extract the operands.
    */

    get_operands(line, operands, &operands_count);

    if (operands == NULL) { 
        log_error("Invalid number of operands in line %d\n\tExpected: 0-2, got: more then 2\n", line_num);
        found_error = 1;
    }

    if (operands_count != (int) op_group) {
        log_error("Invalid number of operands in line %d\n\tExpected: %d, got: %d\n", line_num, op_group, operands_count);
        found_error = 1;
    }

    /* Validate operands are not made by more then 1 word. For example: "r1 r2" is invalid. */
    for (i = 0; i < operands_count; i++) {
        if (strchr(operands[i], ' ') != NULL) {
            log_error("Invalid operand in line %d\n\tOperand: %s at: ...%s \t(missing ',')\n", line_num, operands[i], line);
            found_error = 1;
        }
    }


    if (operands_count == 1) {
        dest = address_mode(operands[0]);
    } else if (operands_count == 2) {
        source = address_mode(operands[0]);
        dest = address_mode(operands[1]);
    } 

    /* Now validate the adress modes are ok */
    if (!valid_command_with_operands(op, dest, source)) {
        log_error("Invalid operands in line %d.\n\t The Operation: %d does not support addressing modes of source/dest operands\n\tDest: %d, Source: %d\n", line_num, op, dest, source);
        found_error = 1;
    }

    /* Start building the first word of instruction */
    first_word = build_first_word(op, source, dest, ic, line_num);
    code_image[*ic - INITIAL_IC_VALUE] = first_word;
    (*ic)++;

    is_source_reg = source == REGISTER || source == POINTER;
    is_dest_reg = dest == REGISTER || dest == POINTER;

    if (is_source_reg && is_dest_reg) { /* Source and dest share a word */
        second_word = build_single_word(operands[0], operands[1], ic, line_num);
        add_to_code_image(code_image, second_word, ic, &found_error);
    } else if (operands_count == 1) {
        second_word = build_additional_word(dest, 1, operands[0], ic, line_num);
        add_to_code_image(code_image, second_word, ic, &found_error);
    } else if (operands_count == 2) {
        second_word = build_additional_word(source, 0, operands[0], ic, line_num);
        add_to_code_image(code_image, second_word, ic, &found_error);

        third_word = build_additional_word(dest, 1, operands[1], ic, line_num);
        add_to_code_image(code_image, third_word, ic, &found_error);
    } 

    safe_free(operationName);
    free_operands(operands, operands_count);

    return !found_error;

}