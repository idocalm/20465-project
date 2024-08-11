#include "data_line.h"

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
    int value;  /* Will store the integer value of the current argument */
    int has_inserted = 0; /* A flag to see if we've inserted any data */

    if (!validate_operand_list(ptr, line_num, 1)) { /* Validate the line */
        return 0; /* Found an error */
    }

    while ((operand = strtok(ptr, ",")) != NULL) {
        
        remove_all_spaces(operand); /* Remove all spaces from the operand so we'll have only the integer with + or - */
        value = is_integer(operand); /* Attempt to parse the value into an int */

        if (value == NON_VALID_INTEGER) {
            log_line_error(line_num, line, "Invalid .data: The operand '%s' is not a valid number", operand);
            return 0; /* Found an error */
        }
        
        if (!validate_int_size(value, SIGNED_15_MIN, UNSIGNED_15_MIN, UNSIGNED_15_MAX)) {
            log_line_error(line_num, line, "Invalid .data: The operand '%s' is out of range", operand);
            return 0; /* Found an error */
        }
        /* Add the value to the machine if it's ok */
        add_to_data_image(data_image, dc, value);
        has_inserted = 1;

        ptr = NULL;
    }

    if (!has_inserted) {
        log_line_error(line_num, line, "Invalid .data: No data was found in the line");
        return 0; /* Found an error */
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
    ptr++; /* Skip the first " */
    while (*ptr != '\0' && *ptr != '\n' && *ptr != '"') { /* While we're not at the end of the string / line */
        add_to_data_image(data_image, dc, *ptr); /* Add the char to the data image */
        ptr++;
    }

    add_to_data_image(data_image, dc, '\0'); /* Don't forget to add the null terminating char ! */

}

/** 
    * @brief Handles an .extern line 
    * @param line The line (starting from the first char after the ".extern")
    * @param labels The labels table
    * @param line_num The line number
    * @return 1 if the line is valid, 0 otherwise
*/

int insert_extern_arguments(char *line, Labels *labels, int line_num) {
    char *label = copy_string_until_space(line);

    if (strlen(label) == 0) { /* Check if the label is empty */
        log_line_error(line_num, line, "Invalid .extern: No argument was found");
        safe_free(label);
        return 0;
    }

    if (!is_label(label)) { /* Check that the extern argument is a valid label */
        log_line_error(line_num, line, "Invalid .extern: The argument '%s' is not a valid label", label);
        safe_free(label);
        return 0;
    }

    if (labels_get(labels, label, EXTERN_LABEL) != NULL) {
        /* Note: the instruction on duplicate definitions as extern are not clear. We've choosen to log a warning and not an error */
        log_warning("Invalid .extern in line %d.\n\t.The argument '%s' was already defined as an extern in the file\n\tYou should remove one of the definitions.\n", line_num, label);
        safe_free(label);
        return 1;
    }

    if (labels_get_any(labels, label) != NULL) { /* Extern can't be defined inside the current file */
        log_line_error(line_num, line, "Invalid .extern: The argument '%s' was already defined as a code/data label", label);
        safe_free(label);
        return 0;
    }

    /* Insert the label as an extern to the label tabel with 0 */
    labels_insert(labels, label, 0, EXTERN_LABEL);
    safe_free(label);
    return 1;

}

/**
    * @brief Handles a data line (either .data, .string, .entry, .extern) - this is the main function for handling data lines
    * @param line The line to handle
    * @param line_num The line number
    * @param dc The data counter
    * @param label a possible already defined label
    * @param labels The labels table
    * @param data_image The data image array
    * @return 1 if the line has any errors, 0 otherwise
*/

int handle_data_line(char *line, int line_num, int *dc, char *label, Labels *labels, machine_word **data_image) {
   
    int found_error = 0;
    char *ptr_line = line;
    char *instruction_name = copy_string_until_space(ptr_line); /* Get the instruction name */
    Instruction instruction = get_instruction(instruction_name); /* Get the instruction "code" */

    if (instruction != ENTRY && instruction != EXTERN && label[0] != '\0') { 
        /* If it's not an .entry or .extern (which don't support labels before them) we add the label to the table */
        LabelEntry *entry = labels_get(labels, label, DATA_LABEL);
        if (entry != NULL) {
            /* Repeating name of a label */
            log_line_error(line_num, line, "Label '%s' was already defined in the file", label);
            found_error = 1;
        } else {
            labels_insert(labels, label, *dc, DATA_LABEL);
        }
    } else if ((instruction == ENTRY || instruction == EXTERN) && label[0] != '\0') { 
        /* As mentioned, we throw a warning and ignore it */
        log_warning("(In line: %d) .entry and .extern instructions don't support defining a label beforehand ('%s')\n", line_num, label);
    }

    ptr_line += strlen(instruction_name); /* Move the line pointer past the instruction */
    safe_free(instruction_name);
    
    /* Skip any spaces after the instruction */
    skip_spaces(&ptr_line);

    /* Handle the line according to the instruction type */
    
    if (instruction == STRING) {
        validate_string_instruction(ptr_line, line_num, &found_error);
        if (!found_error) 
            insert_string_arguments(ptr_line, dc, data_image, line_num);
    } else if (instruction == DATA) {
        /* Handle the data line in a separate function */
        if (!insert_data_arguments(ptr_line, dc, data_image, line_num)) 
            found_error = 1;
    } else if (instruction == EXTERN) {
        /* Handle the extern line in a separate function */
        if (!insert_extern_arguments(ptr_line, labels, line_num))
            found_error = 1; 
    }

    /* If we found an error, return 1 */
    if (found_error) {
        return 1;
    }

    return 0; /* No errors */

}