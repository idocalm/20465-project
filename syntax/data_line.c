#include "data_line.h"

/**
    * @brief Given some data, the function inserts it to the data image
    * @param data_image The data image array 
    * @param dc The data counter
    * @param data The data to insert in the word
*/

void add_data_to_image(machine_word **data_image, int *dc, int data) {
    data_image[*dc] = (machine_word *) safe_malloc(sizeof(machine_word));
    data_image[*dc]->data = data;
    (*dc)++;
}


/**
    * @brief Parses a .data line into the data image while validating as we go
    * @param line The line to parse
    * @param dc The data counter
    * @param data_image The data image array
    * @param line_num The line number
*/

int insert_data_arguments(char *line, int *dc, machine_word **data_image, int line_num) {
    char *operand = NULL; /* Will store the current argument in the .data line */
    char *ptr = line; /* A pointer for validating without destroying the *line */
    char *last_char = line + strlen(line) - 1; /* A pointer to the last character in the line (for validation) */
    int value;  /* Will store the integer value of the current argument */

    /* Check if after skipping the spaces there's an illegal ',' */
    if (line[0] == ',') {
        log_error("Invalid .data in line %d.\n\tData arguments starts with a comma.\n", line_num);
        return 0;
    }

    /* Skip spaces from the end going backwards */
    while (isspace(*last_char)) {
        last_char--;
    }

    /* Check if after skipping the data ends with an ',' */
    if (*last_char == ',') {
        log_error("Invalid .data in line %d.\n\tData arguments ends with a comma.\n", line_num);
        return 0;
    }

    /* Check for no double commas through the argument */
    while ((operand = strchr(ptr, ',')) != NULL) {
        operand++; 
        skip_spaces(&operand); /* Skip spaces after the comma because ,     , is also invalid */
        if (*operand == ',') {
            log_error("Invalid .data in line %d.\n\tData arguments are separated by a double comma.\n", line_num);
            return 0; /* Found an error */
        }
        ptr = operand + 1; /* Move pas the , */
    }   

    /* Go back to the start, now build the actual data */
    ptr = line;

    while ((operand = strtok(ptr, ",")) != NULL) {
    
        remove_all_spaces(operand); /* Remove all spaces from the operand so we'll have only the integer with + or - */
        value = is_integer(operand); /* Attempt to parse the value into an int */

        if (value == NON_VALID_INTEGER) {
            log_error("Invalid .data in line %d\n\t. Data '%s' is not an integer.\n", line_num,  operand);
            return 0; /* Found an error */
        }

        /* Add the value to the machine if it's ok */
        add_data_to_image(data_image, dc, value);

        ptr = NULL;
    }

    return 1; /* No errors */
}

/**
    * @brief Parses a VALID .string line into the data image 
    * @param line The line (starting from the first char after the ")
    * @param dc The data counter
    * @param data_image The data image array
    * @param line_num The line number
*/
void insert_string_arguments(char *line, int *dc, machine_word **data_image, int line_num) {
    
    char *ptr = line; 
    while (*ptr != '\0' && *ptr != '\n' && *ptr != '"') { /* While we're not at the end of the string / line */
        add_data_to_image(data_image, dc, *ptr); /* Add the char to the data image */
        ptr++;
    }

    add_data_to_image(data_image, dc, '\0'); /* Don't forget to add the null terminating char ! */
}

/** 
    * @brief Handles an .extern line 
    * @param line The line (starting from the first char after the ".extern")
    * @param line_num The line number
    * @param labels The labels table
    * @return 1 if the line is valid, 0 otherwise
*/

int insert_extern_arguments(char *line, Labels *labels, int line_num) {
    char *entry_label = (char *) safe_malloc(MAX_LABEL_SIZE);   
    copy_string_until_space(entry_label, line);
    entry_label = (char *) safe_realloc(entry_label, strlen(entry_label) + 1);
    entry_label[strlen(entry_label)] = '\0';

    if (!is_label(entry_label)) { /* Check that the extern argument is a valid label */
        log_error("Invalid .extern in line %d.\n\tThe argument '%s' can't be a valid label.\n", line_num, entry_label);
        safe_free(entry_label);
        return 0;
    }

    if (labels_get(labels, entry_label, EXTERN_LABEL) != NULL) {
        /* Note: the instruction on duplicate definitions as extern are not clear. We've choosen to log a warning and not an error */
        log_warning("Invalid .extern in line %d.\n\t.The argument '%s' was already defined as an extern in the file\n\tYou should remove one of the definitions.\n", line_num, entry_label);
        safe_free(entry_label);
        return 1;
    }

    if (labels_get_any(labels, entry_label) != NULL) { /* Extern can't be defined inside the current file */
        log_error("Invalid .extern in line %d.\n\t The argument '%s' was already defined as a code/data label\n", line_num, entry_label);
        safe_free(entry_label);
        return 0;
    }

    /* Insert the label as an extern to the label tabel with 0 */
    labels_insert(labels, entry_label, 0, EXTERN_LABEL);
    safe_free(entry_label);
    return 1;

}

/**
    * @brief Handles a data line (either .data, .string, .entry, .extern) - this is the main function for handling data lines
    * @param line The line to handle
    * @param line_num The line number
    * @param ic The instruction counter
    * @param dc The data counter
    * @param labels The labels table
    * @param data_image The data image array
    * @return 1 if the line has any errors, 0 otherwise
*/

int handle_data_line(char *line, int line_num, int *ic, int *dc, Labels *labels, machine_word **data_image) {

    Instruction instruction;
    char *instruction_name = NULL;
    int found_error = 0;
    char *ptr = NULL;
    char *label = (char *) safe_malloc(MAX_LABEL_SIZE);

    skip_spaces(&line);

    /* #1 - Extract the label, if exists. */
    
    if (is_label_error(line, line_num, label, 1)) { /* Check if the label is valid (doesn't exists / exists & valid) */
        /* A note: once the label has some sort of error we can't cotinue 
                   because we don't know how to analyze the sentence and where the op / operands are
           So we return 1 (an error) immediately
        */
        printf("Error at line %d\n", line_num);
        safe_free(label);
        return 1; 
    }


    if (label[0] != '\0') { /* The is_label_error function sets the first char to be \0 if somethings wrong with the label */
        /* Move the line pointer past the label */
        line += strlen(label) + 1;
    }


    /* Skip any spaces before the acutal instruction */
    skip_spaces(&line);


    instruction_name = get_instruction_name(line); /* Get the instruction name */
    instruction = get_instruction(instruction_name); /* Get the instruction "code" */

    if (instruction != ENTRY && instruction != EXTERN && label[0] != '\0') { 
        /* If it's not an .entry or .extern (which don't support labels before them) we add the label to the table */
        LabelEntry *entry = labels_get(labels, label, DATA_LABEL);
        if (entry != NULL) {
            /* Repeating name of a label */
            log_error("Line %d.\n\tLabel '%s' was already defined in the file.\n", line_num, label);
            found_error = 1;
        } else {
            labels_insert(labels, label, *dc, DATA_LABEL);
        }
    } else if ((instruction == ENTRY || instruction == EXTERN) && label[0] != '\0') { 
        /* As mentioned, we throw a warning and ignore it */
        log_warning("(In line: %d) .entry and .extern instructions don't support defining a label beforehand ('%s')\n", line_num, label);
    }

    safe_free(label); /* We don't need the label anymore */

    line += strlen(instruction_name); /* Move the line pointer past the instruction */
    safe_free(instruction_name);
    
    /* Skip any spaces after the instruction */
    skip_spaces(&line);

    /* Handle the line according to the instruction type */
    if (instruction == STRING) {

        /* Validate that the string starts with a " */
        if (*line != '"') {
            log_error("Invalid .string command.\n\tInvalid argument in line %d.\n\tExpected: \"<string>\", got: %s\n", line_num, line);
            return 1;
        }

        line++;
        ptr = line + 1; /* Skip the " */

        while (!isspace(*ptr) && *ptr != '"') {
            ptr++;
        } 

        /* Validate that the string ends with a " */
        if (*ptr != '"') {
            log_error("Invalid .string command.\n\tInvalid argument in line %d.\n\tExpected: \"<string>\", got: %s\n", line_num, line);
            return 1;
        }

        ptr++; /* Skip the " */

        skip_spaces(&ptr);

        /* Validate that there are no extra characters after the string */
        if (*ptr != '\0' && *ptr != '\n') {
            log_error("Invalid .string command.\n\tInvalid arguments in line %d.\n\tReceived extra characters after closure.\n", line_num);
            return 1;
        }

        /* Let this function handle inserting the letters */
        insert_string_arguments(line, dc, data_image, line_num);

    } else if (instruction == DATA) {

        /* Handle the data line in a separate function */
        if (!insert_data_arguments(line, dc, data_image, line_num)) {
            found_error = 1;
        }

    } else if (instruction == EXTERN) {

        /* Handle the extern line in a separate function */
        if (!insert_extern_arguments(line, labels, line_num)) {
            found_error = 1;
        }
        
    }


    /* If we found an error, return 1 */
    if (found_error) {
        return 1;
    }

    return 0; /* No errors */

}