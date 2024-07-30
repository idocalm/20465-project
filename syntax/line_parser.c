#include "line_parser.h"

char *extract_label(char *p) {
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

void get_operands(char *line, char **operands, int *operandsCount) {
    int i = 0;
    char *operand = NULL;

    /* split the line by ',' */
    while ((operand = strtok(line, ",")) != NULL) {

        if (i == MAX_OPERANDS + 1) {
            return;
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

int is_valid_label(char *label, List *p_macros) {

    char *ptr = label;

    while (*ptr != '\0' && *ptr != '\n' && *ptr != ':') {
        if (!isalpha(*ptr) && !isdigit(*ptr)) {
            return 0;
        }
        ptr++;
    }

    char *labelCopy = (char *) safe_malloc(ptr - label + 1);
    strncpy(labelCopy, label, ptr - label);
    labelCopy[ptr - label] = '\0';


    if (!is_reserved_word(label) && list_get(p_macros, label) == NULL && strlen(label) <= MAX_LABEL_SIZE) {
        safe_free(labelCopy);
        return 1;
    }

    safe_free(labelCopy);
    return 0;

}


AddressMode find_addressing_mode(char *operand, List *p_macros) {
    if (operand[0] == '#' && is_integer(operand + 1)) { /* Immediate addressing has '#' followed by a number */
        return IMMEDIATE; 
     } else if (get_register(operand) != UNKNOWN_REGISTER) { /* 'Register addressing' has 'r' followed by a number between 0-7 */
        return REGISTER;
    } else if (operand[0] == '*' && get_register(operand + 1) != UNKNOWN_REGISTER) { /* 'Relative addressing' has '*' followed by a register */
        return RELATIVE;
    } else if (is_valid_label(operand, p_macros)) { /* 'Direct addressing' is a label */
        return DIRECT;
    }

    return UNKNOWN_ADDRESS;

}


void debug_word(first_word *p_word, int *p_ic) {


    printf("%d\t\t", *p_ic);
    int i; 
   
    red_text();
    for (i = 3; i >= 0; i--) {
        printf("%u", (p_word->opcode >> i) & 1);
    }

    green_text();
    for (i = 3; i >= 0; i--) {
        printf("%u", (p_word->source >> i) & 1);
    }

    yellow_text();
    for (i = 3; i >= 0; i--) {
        printf("%u", (p_word->dest >> i) & 1);
    }

    reset_text();
    for (i = 2; i >= 0; i--) {
        printf("%u", (p_word->ARE >> i) & 1);
    }

    printf("\n");


}

void build_immediate_word(additional_word *p_word, AddressMode mode, char *operand) {
    int value = atoi(operand + 1);

    printf("operand: %s\n", operand);

    p_word->ARE = 1 << 2; /* The first word of instruction always has A = 1, R = 0, E = 0 */
    p_word->value = parse_int(operand + 1);


}

void build_additional_word(additional_word *p_word, AddressMode mode, int isDest, char *operand, int *p_ic) {
    if (mode == IMMEDIATE) {
        build_immediate_word(p_word, mode, operand);
        debug_word((first_word *) p_word, p_ic);
    }

    *p_ic += 1;  

}
void build_first_word(first_word *p_word, Operation op, char **operands, int operandsCount, List *p_labels, List *p_macros, int *p_ic, int lineNum) {

    AddressMode dest, source; 
    
    
    if (operandsCount == 1) {
        dest = find_addressing_mode(operands[0], p_macros);
        source = UNKNOWN_ADDRESS;
    } else if (operandsCount == 2) {
        source = find_addressing_mode(operands[0], p_macros);
        dest = find_addressing_mode(operands[1], p_macros);
    } else {
        dest = UNKNOWN_ADDRESS;
        source = UNKNOWN_ADDRESS;
    } 

    /* Validate operands by their addressmode */

    if (!valid_command_with_operands(op, dest, source)) {
        log_error("Invalid operands in line %d.\n\t The Operation: %d does not support addressing modes of source/dest operands\n\tDest: %d, Source: %d\n", lineNum, op, dest, source);
        return;
    }

    if (dest == IMMEDIATE) {

        char *operand = operandsCount == 1 ? operands[0] + 1 : operands[1] + 1;
        if (is_12_bit_number(operand) == 0) {
            log_error("Invalid immediate operand in line %d.\n\tThe number: %s is not a 12 bit number.\n", lineNum, operand);
            return;
        }
    } 

    if (source == IMMEDIATE) {
        if (is_12_bit_number(operands[0] + 1) == 0) {
            log_error("Invalid immediate operand in line %d.\n\tThe number: %s is not a 12 bit number.\n", lineNum, operands[0]);
            return;
        }
    }
    
    p_word->ARE = 1 << 2; /* The first word of instruction always has A = 1, R = 0, E = 0 */

    p_word->opcode = op;
    if (dest != UNKNOWN_ADDRESS) {
        p_word->dest |= (1 << dest); 
    } 

    if (source != UNKNOWN_ADDRESS) {
        p_word->source |= (1 << source); 
    }

    debug_word(p_word, p_ic);
    *p_ic += 1;

    if (operandsCount == 2) {
        additional_word *p_first_op = (additional_word *) safe_malloc(sizeof(additional_word));
        build_additional_word(p_first_op, source, 0, operands[0], p_ic);
        additional_word *p_second_op = (additional_word *) safe_malloc(sizeof(additional_word));
        build_additional_word(p_second_op, dest, 1, operands[1], p_ic);
    } else if (operandsCount == 1) {
        additional_word *p_additional = (additional_word *) safe_malloc(sizeof(additional_word));
        build_additional_word(p_additional, dest, 1, operands[0], p_ic);
    }

    printf("\n");




}   

void handle_directive_line(char *line, int lineNum, int *p_ic, List *p_labels, List *p_macros, int isFirstWord) {


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

    label = extract_label(line);

    if (label != NULL && !is_valid_label(label, p_macros)) {
        return; 
    }

    if (label != NULL) {
        line += strlen(label) + 1;
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

    if (operands == NULL) { /* No operands */
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

    /* Build the instruction */
    first_word *p_word = (first_word *) safe_malloc(sizeof(first_word));
    build_first_word(p_word, op, operands, operandsCount, p_labels, p_macros, p_ic, lineNum);





}

void handle_instruction_line(char *line) {
    /*printf("Handling instruction line: %s\n", line);*/ 
}

