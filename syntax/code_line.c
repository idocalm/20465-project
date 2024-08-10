#include "code_line.h"

/** 
    * @brief Adds a word that was already encoded to the machines code image
    * @param code_image - the code image array 
    * @param word - the word we're going to add
    * @param ic - the instruction counter
    * @param found_error - a flag that indicates if the memory is about to go overflow    

*/

void add_to_code_image(machine_word **code_image, machine_word *word, int *ic, int *found_error) {
    /* Check we're not writing pass memory */
    if (*ic - INITIAL_IC_VALUE >= ASSEMBLER_MAX_CAPACITY) { /* We're out of memory */
        log_error("The system code image is full. No more words can be written.\n\tCurrent instruction counter: %d.\n", *ic);
        safe_free(word);
        *found_error = 1;
        return;
    }

    /* Add the word to the code image with - INITIAL_IC_VALUE so we don't waste cells in the array */
    code_image[*ic - INITIAL_IC_VALUE] = word;
    (*ic)++;
}

void extract_address_modes(char **operands, int operands_count, AddressMode *dest, AddressMode *source, int line_num, int *found_error) {
    /* 
        Note that the validation of the operands themselves are done inside address_mode function
        and if there's an error we'll get -1 and the found_error flag will be set to 1
    */

    if (operands_count == 1) {
        *dest = address_mode(operands[0], 1, line_num, found_error);
    } else if (operands_count == 2) {
        *source = address_mode(operands[0], 1, line_num, found_error);
        *dest = address_mode(operands[1], 1, line_num, found_error);
    } 
}



/**
    * @brief Builds a single word when BOTH source and dest are registers (or "pointers")
    * @param source - the source operand
    * @param dest - the dest operand
    * @param ic - the instruction counter
    * @return a pointer to the word
*/

machine_word *build_single_word(char *source, char *dest, int *ic) {
    Register source_reg, dest_reg; /* Actual Register values */
    machine_word *word = (machine_word *) safe_malloc(sizeof(machine_word)); /* The word we're building */

    word->data = 0; /* First we reset the word */

    if (*source == '*') { /* Skip the '*' */
        source++; 
    }

    if (*dest == '*') { /* Skip the '*' */
        dest++; 
    }

    source_reg = get_register(source);
    dest_reg = get_register(dest);

    word->data |= (1 << 2); /* Turning 'A' on (according to page 34 in the booklet) */
    word->data |= (source_reg << 6); /* Source register goes in bits 6-8 */
    word->data |= (dest_reg << 3); /* Dest register goes in bits 3-5 */

    return word;
}


/**
    * @brief Builds a word for an additional operand when it's an immediate value (like +-5) or a register (/pointer)
    * @param mode - the address mode of the operand
    * @param is_dest - is the operand a dest operand?
    * @param operand - the operand itself
    * @param ic - the instruction counter
*/

machine_word *build_additional_word(AddressMode mode, int is_dest, char *operand, int *ic) {

    machine_word *word = (machine_word *) safe_malloc(sizeof(machine_word));

    word->data = 0; /* First we reset the word */

    if (mode == IMMEDIATE) {
        int value = is_integer(operand + 1); /* Skip the # */
        
        word->data |= (1 << 2); /* Turning 'A' on (page 43 in the booklet) */
        word->data |= (value << 3); /* The value goes in bits 3-14 - a 12 bit number. */

        return word;
    } 
  
    if (mode == REGISTER) {
        Register reg = get_register(operand);

        word->data |= (1 << 2); /* Turning 'A' on */

        if (is_dest) {
            word->data |= (reg << 3); /* Destination operands go to bits 3-5 */
        } else {
            word->data |= (reg << 6); /* Source goes to 6-8 */
        }

        return word;

    } 
    
    if (mode == POINTER) {
        Register reg = get_register(operand + 1);

        word->data |= (1 << 2);

        if (is_dest) {
            word->data |= (reg << 3);
        } else {
            word->data |= (reg << 6);
        }

        return word;

   }

    return word;

}

/**
    * @brief Builds the first word of an instruction
    * @param op - the operation
    * @param source - the source operand
    * @param dest - the dest operand
    * @param ic - the instruction counter
    * @return a pointer to the word
*/

machine_word *build_first_word(Operation op, AddressMode source, AddressMode dest, int *ic) {
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

/**
    * @brief Validates that the number of an immediate value is not over 12 bits
    * @param mode - the address mode of the operand
    * @param operand - the operand
    * @param line_num - the line number
    * @param found_error - a flag to turn on if there's an error
*/

void validate_immediate(AddressMode mode, char *operand, int line_num, int *found_error) {
    if (mode == IMMEDIATE) {
        int value = is_integer(operand + 1); /* Skip the # */
        if (value > MAX_12_BIT_NUMBER || value < MIN_12_BIT_NUMBER) {
            log_error("Invalid immediate value in line %d\n\tValue: %d is out of range.\n", line_num, value);
            *found_error = 1;
        }
    }
}


/**
    * @brief Handles a line of code and builds the machine code for it
    * @param line - the line of code
    * @param line_num - the line number
    * @param ic - the instruction counter
    * @param labels - the labels table
    * @param code_image - the code image array
    * @return 1 if an error was found, 0 otherwise
*/

int handle_code_line(char *line, int line_num, int *ic, Labels *labels, machine_word **code_image) {

    char *operation_name = NULL; 
    char **operands = safe_malloc(MAX_OPERANDS * sizeof(char *)); /* Storing up to 2 extra operands of the first word */
    int operands_count = 0; /* The number of operands we have */

    /* Destination and source address modes */
    AddressMode dest = UNKNOWN_ADDRESS; 
    AddressMode source = UNKNOWN_ADDRESS;

    int i = 0; 
    int is_source_reg, is_dest_reg; /* Used to determine if we need to build a single word or 2 */
    Operation op; 
    OperationGroup op_group; /* The group (i.e: 0/1/2 operands) required by the operation */

    /* Possible words to build */
    machine_word *first_word = NULL;
    machine_word *second_word = NULL;
    machine_word *third_word = NULL;

    int found_error = 0; /* Have we found any kind of error through the process? */

    char label[MAX_LABEL_SIZE + 1]; /* A label that appeared before the command (if exists) */
    label[MAX_LABEL_SIZE] = '\0';

    skip_spaces(&line); /* Skip any spaces before possible label def */

    if (is_label_error(line, line_num, label, 1)) { /* Check if the label is valid (doesn't exists / exists & valid) */
        /* A note: once the label has some sort of error we can't cotinue 
                   because we don't know how to analyze the sentence and where the op / operands are
           So we return 1 (an error) immediately
        */
        safe_free(operands);

        return 1;
    }

    if (label[0] != '\0') { /* The is_label_error function sets the first char to be \0 if somethings wrong with the label */
        LabelEntry *entry = labels_get_any(labels, label); 
        if (entry != NULL) { /* Check if the label is already defined */
            log_error("Label '%s', in line %d is already defined in the file.\n", label, line_num);
            found_error = 1;
        }

        line += strlen(label) + 1; /* Move over the label */
        labels_insert(labels, label, *ic, CODE_LABEL); /* Insert the label to the labels table! Profit! */
    }

    skip_spaces(&line); /* Skip any spaces before the operation */

    /* Extract information about the operation */
    operation_name = get_op_name(line);
    op = get_operation(operation_name);

    if (op == UNKNOWN_OPERATION) { /* We're unfamiliar with the op (received -1) */
        log_error("Invalid operation name '%s' in line %d.\n", operation_name, line_num);
        safe_free(operation_name);
        free_operands(operands, operands_count);
        return 1;
    }

    op_group = get_operation_group(op);
    line += strlen(operation_name); /* Skip the operation name and move to the operands */

    skip_spaces(&line); /* Skip any spaces before the operands */
    
    get_operands(line, operands, &operands_count, line_num); /* Extract the operands */

    if (operands == NULL) { /* The function is designed to return NULL if there were more then 2 operands */
        log_error("Invalid number of operands in line %d\n\tExpected: 0-2, received: more then 2.\n", line_num);
        found_error = 1;
    }

    /* Validate that the number of operands matches the operation */
    if (operands_count != (int) op_group) {
        log_error("Invalid number of operands in line %d\n\tExpected: %d, received: %d.\n", line_num, op_group, operands_count);
        found_error = 1;
    }

    /* Validate operands are not made by more then 1 word. For example: "r1 r2" is invalid. */
    for (i = 0; i < operands_count; i++) {
        if (strchr(operands[i], ' ') != NULL) {
            log_error("Invalid operand '%s' in line %d\n\t(missing ',')\n",operands[i], line_num);
            found_error = 1;
        }
    }

    extract_address_modes(operands, operands_count, &dest, &source, line_num, &found_error); /* Extract the address modes to dest/source */
    /* Now validate the adress modes are ok with the operation */
    if (!valid_command_with_operands(op, dest, source)) {
        log_error("Invalid operands in line %d.\n\tThe Operation: %d does not support one or more operands addressing modes.\n\tDest: %d\tsource: %d.\n", line_num, op, dest, source);
        found_error = 1;
    }

    /* Start building the first word of instruction */
    first_word = build_first_word(op, source, dest, ic);
    code_image[*ic - INITIAL_IC_VALUE] = first_word;
    (*ic)++;

    is_source_reg = source == REGISTER || source == POINTER;
    is_dest_reg = dest == REGISTER || dest == POINTER;

    if (operands_count == 1) {
        validate_immediate(dest, operands[0], line_num, &found_error); /* Validate the immediate value if exists */
    }
    else if (operands_count == 2) {
        printf("Operand 1: %s\n", operands[0]);
        validate_immediate(dest, operands[1], line_num, &found_error); /* Validate the immediate value if exists */
        validate_immediate(source, operands[0], line_num, &found_error); /* Validate the immediate value if exists */
    }



    if (found_error) { /* If we found an error we don't want to continue */
        safe_free(operation_name);
        free_operands(operands, operands_count);
        return 1;
    }

    /* -- Word building -- */

    if (is_source_reg && is_dest_reg) { /* Source and dest share a word */
        second_word = build_single_word(operands[0], operands[1], ic);
        add_to_code_image(code_image, second_word, ic, &found_error);
    } else if (operands_count == 1) { /* Only need to build 1 additional word */
        second_word = build_additional_word(dest, 1, operands[0], ic);
        add_to_code_image(code_image, second_word, ic, &found_error);
    } else if (operands_count == 2) { /* Need to build 2 additional words */
        second_word = build_additional_word(source, 0, operands[0], ic);
        add_to_code_image(code_image, second_word, ic, &found_error);

        third_word = build_additional_word(dest, 1, operands[1], ic);
        add_to_code_image(code_image, third_word, ic, &found_error);
    } 

    /* Free any memory we used */
    safe_free(operation_name);
    free_operands(operands, operands_count);

    return found_error; /* 1 - errors found, 0 - no errors found */
}