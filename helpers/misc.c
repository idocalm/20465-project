#include "misc.h"

/**
    * @brief Validates a given operand list (checks for double commas, commas at the beginning / end of the list)
    * @param line - the string
    * @param line_num - the line number (for error reporting)
    * @param report_error - a flag to indicate if we should report the error
    * @return 1 if the operand list is valid, 0 otherwise

    This is useful basically for each code line or for .data lines
*/
int validate_operand_list(char *line, int line_num, int report_error) {
    char *operand = NULL;
    char *last = NULL;
    char *p_line = line;

    skip_spaces(&p_line);
    last = p_line + strlen(line) - 1;

    /* Check if after skipping the spaces there's an illegal ',' */
    if (p_line[0] == ',') {
        if (report_error) 
            log_line_error(line_num, line,"Invalid comma: The argument list starts with a comma");
        return 0;
    }

    /* Skip spaces from the end going backwards */
    while (isspace(*last)) {
        last--;
    }

    /* Check if after skipping all the arguments, the line ends with an ',' */

    if (*last == ',') {
        if (report_error)
            log_line_error(line_num, line, "Invalid comma: The argument list ends with a comma");
        return 0;
    }

    /* Check for no double commas */
    while ((operand = strchr(p_line, ',')) != NULL) {
        operand++; 
        skip_spaces(&operand); /* Skip spaces after the comma because ,     , is also invalid */
        if (*operand == ',') {
            if (report_error)
                log_line_error(line_num, line, "Invalid comma: The argument list contains a double comma");
            return 0; /* Found an error */
        }
        p_line = operand + 1; /* Move pas the , */
    }

    return 1;
}

/**
    * @brief Splits a given string into operands and reports for double / invalid commas
    * @param line - the string
    * @param operands - already allocated array to store operands
    * @param operands_count - the number of operands we will find
    * @param line_num - the line number (for error reporting)
*/

int get_operands(char *line, char **operands, int *operands_count, int line_num) {
    int i = 0;
    char *operand = NULL;

    if (!validate_operand_list(line, line_num, 1)) { /* Check if the operand list is valid (this will report errors) */
        return 0;
    }

    /* split line by ',' */
    while ((operand = strtok(line, ",")) != NULL) {

        if (i == MAX_OPERANDS + 1) { /* We can't have more then 2 operands */
            return 0;
        }

        operands[i] = (char *) safe_malloc(strlen(operand) + 1);
        remove_all_spaces(operand); /* Remove all spaces from the operand */
        strcpy(operands[i], operand);

        i++;
        line = NULL; 
    }
    *operands_count = i;

    return 1; 
}

/**
    * @brief Validates the size of an integer
    * @param value The integer to validate
    * @param line_num The line number
    * @return 1 if the integer is valid, 0 otherwise
*/

int validate_int_size(int value, int signed_min, int unsigned_min, int unsigned_max) {
    
    /*
        If the value is positive, we need to check if it's between the unsigned_min and unsigned_max. That's simple.
        If it's negative, we need to check it's not smaller than the signed_min. 
    */

    if (value >= 0) {
        return value >= unsigned_min && value <= unsigned_max;
    }

    return value >= signed_min; 
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
        if (!validate_int_size(value, SIGNED_12_MIN, UNSIGNED_12_MIN, UNSIGNED_12_MAX)) {
            log_line_error(line_num, operand, "Invalid immediate value - out of range");
            *found_error = 1;
        }
    }
}

/**
    * @brief Make sure that the .string argument is valid and ready to be encoded.
    * @param line The line (starting before '"')
    * @param line_num The line number
    * @param found_error A flag to indicate if an error was found
*/

void validate_string_instruction(char *line, int line_num, int *found_error) {
    char *ptr = line;
    skip_spaces(&ptr);
    
    /* Validate that the string starts with a " */
    if (*ptr != '"') {
        log_line_error(line_num, line, "Invalid .string command: The string must start with a \"");
        *found_error = 1;
        return; 
    }
    ptr += 2; /* Skip the " */

    /* Skip the inner charcters */
    while (*ptr != '\n' && *ptr != '\0' && *ptr != '"')
        ptr++;

    /* Validate that the string ends with a " */
    if (*ptr != '"') {
        log_line_error(line_num, line, "Invalid .string command: The string must end with a \"");
        *found_error = 1;
        return; 
    }

    ptr++; /* Skip the " */
    skip_spaces(&ptr);

    /* Validate that there are no extra characters after the string */
    if (*ptr != '\0' && *ptr != '\n') {
        log_line_error(line_num, line, "Invalid .string command. Extra characters after '\"'");
        *found_error = 1;
        return; 
    }

    return; 
}

/**
    * @brief Given some data, the function inserts it to the data image
    * @param data_image The data image array 
    * @param dc The data counter
    * @param data The data to insert in the word
*/

void add_to_data_image(machine_word **data_image, int *dc, int data) {
    data_image[*dc] = (machine_word *) safe_malloc(sizeof(machine_word));
    data_image[*dc]->data = data;
    (*dc)++;
}

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

/**
    * @brief Extracts the address modes from the operands
    * @param operands - the operands array
    * @param operands_count - the number of operands
    * @param dest - the destination address mode
    * @param source - the source address mode
    * @param line_num - the line number
    * @param found_error - a flag to indicate if an error was found
*/

void extract_address_modes(char **operands, int operands_count, AddressMode *dest, AddressMode *source, int line_num, int *found_error) {
    
    /* 
        Note that the validation of the operands themselves are done inside address_mode function
        and if there's an error we'll get -1 and the found_error flag will be set to 1 as wel l 
    */

    if (operands_count == 1) {
        *dest = address_mode(operands[0], 1, line_num, found_error);
    } else if (operands_count == 2) {
        *source = address_mode(operands[0], 1, line_num, found_error);
        *dest = address_mode(operands[1], 1, line_num, found_error);
    } 
}

