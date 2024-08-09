#include "second_pass.h"
/**
    * @brief Tries to re-encode a word in the machine, after we know the label that's missing.
    * @param operand The operand (a label)
    * @param labels The labels table
    * @param extern_usage The extern usage list
    * @param code_image The code image
    * @param ic_counter The instruction counter
    * @param line_num The line number
*/

int replace_label(char *operand, Labels *labels, List *extern_usage, machine_word **code_image, int ic_counter, int line_num) {
    LabelEntry *entry = labels_get_any(labels, operand);

    if (entry == NULL) {
        log_error("Label '%s' referenced in line %d is not defined anywhere in the file.\n", operand, line_num);
        return 0;
    }

    if (entry->type == EXTERN_LABEL) {
        /* It's currently all 0. Turn on the E bit (first from right )*/
        code_image[ic_counter - INITIAL_IC_VALUE]->data |= (1 << 0);
        list_insert_integer(extern_usage, entry->key, ic_counter);
    } else {
        /* It's currently all 0. Turn on the R bit (second from right )*/
        code_image[ic_counter - INITIAL_IC_VALUE]->data |= (1 << 1);
    }
    
    /* Value goes from the 3rd bit */
    code_image[ic_counter - INITIAL_IC_VALUE]->data |= (entry->value << 3);


    return 1;
}

/**
    * @brief Handles the .entry line
    * @param line The line
    * @param labels The labels table
    * @param line_num The line number
    * @param found_error A flag to indicate if an error was found
*/

void handle_entry_line(char *line, Labels *labels, int line_num, int *found_error) {

    char *value, *validate;
    LabelEntry *entry;

    /* Move over the .entry and skip any spaces before the label */
    line += strlen(".entry");
    skip_spaces(&line);

    value = (char *) safe_malloc(MAX_LABEL_SIZE + 1);
    copy_string_until_space(value, line);

    /* Check that the label is even a valid label */
    if (!is_label(value)) {
        log_error("Invalid .entry in line %d.\n\tThe argument '%s' can't be a valid label.\n", line_num, value);
        safe_free(value);
        *found_error = 1;
        return;
    }

    validate = line + strlen(value) + 1; 

    skip_spaces(&validate);

    /* Check that there are no extra characters after the label */

    if (*validate != '\0' && *validate != '\n') {
        log_error(".entry operation has more then one operand.\n\t (externous characters / second label) in line %d.\n", line_num);
        safe_free(value);
        *found_error = 1;
        return;
    }

    /* Check that the label is not already defined as an extern */

    if (labels_get(labels, value, EXTERN_LABEL) != NULL) {
        log_error("Label '%s' in line %d was already defined as an .extern in this file.\n", value, line_num);
        *found_error = 1;
        safe_free(value);
        return;
    }

    /* Check that the label is not already defined as an entry */

    if (labels_get(labels, value, ENTRY_LABEL) != NULL) {
        log_warning("Invalid .entry in line %d.\n\t.The argument '%s' was already defined as an entry in the file\n\tYou should remove one of the definitions.\n", line_num, value);
        safe_free(value);
        return;
    }

    /* Check that the label is defined SOMEWHERE in the file */

    if (labels_get(labels, value, CODE_LABEL) == NULL && labels_get(labels, value, DATA_LABEL) == NULL) {   
        log_error("Label '%s' referenced in .entry on line %d is not defined in this file.\n", value, line_num);
        *found_error = 1;
        safe_free(value);
        return;
    }

    /* Insert the label as an entry to the label table */
    entry = labels_get_any(labels, value);
    labels_insert(labels, value, entry->value, ENTRY_LABEL);

    /* Free memory */
    safe_free(value);
}

/**
    * @brief Finds the address modes of the operands
    * @param operands The operands array
    * @param operands_count The number of operands
    * @param dest The destination address mode (to update)
    * @param source The source address mode (to update )
*/

void find_address_modes(char **operands, int operands_count, AddressMode *dest, AddressMode *source) {
    if (operands_count == 1) {
        /* When we have only one operand we update just the destination operand. */
        *dest = address_mode(operands[0], 0, 0, NULL);
        *source = UNKNOWN_ADDRESS;
    } else if (operands_count == 2) {
        /* Or update both if we have 2 operands. */
        *source = address_mode(operands[0], 0, 0, NULL);
        *dest = address_mode(operands[1], 0, 0, NULL);
    } 
}

/**
    * @brief The second pass of the assembler
    * @param file_name The file name
    * @param labels The labels table
    * @param extern_usage The extern usage list
    * @param code_image The code image
    * @param data_image The data image
    * @param first_pass_error A flag to indicate if an error was found in the first pass
    * @return NO_PASS_ERROR if no error was found, or FOUND_ERROR if an error was found. 
*/

PassError second_pass(char *file_name, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int first_pass_error) {

    /* Open the file with reading perm */
    FILE *input_file = open_file(file_name, "r");

    int line_num = 0; /* To track where we are */
    int found_error = 0; /* Have we found an error in the second pass? */
    char line[MAX_LINE_SIZE + 2]; /* Line buffer */
    char *p_line; 

    int ic_counter = INITIAL_IC_VALUE; /* Instruction counter (here we begin again from the initial val )*/
    char *operation_name; 
    int is_dest_reg, is_source_reg; /* We use these to check if the encoding will use a signular word for 2 operands */

    /* Operands information */
    char **operands;
    int operands_count;
    AddressMode dest, source;
    
    /* Label information (for a possible one/ if exists)*/
    char label[MAX_LABEL_SIZE + 1];
    label[MAX_LABEL_SIZE] = '\0';

    log_info("Intialized second pass in file %s\n", file_name);

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) {
        line_num++;
        p_line = line;

        /* Skip any unimportant spaces */
        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n') { /* Empty lines (or possibly comments) */
            continue;
        }

        if (strstr(p_line, ".extern") != NULL || strstr(p_line, ".data") != NULL || strstr(p_line, ".string") != NULL) { 
            /* Already handled in first pass */
            continue;
        }

        if (is_label_error(p_line, line_num, label, 0)) {
            /* If there's an error with the label we can't continue */
            found_error = 1;
            continue;
        }

        if (label[0] != '\0') { /* As mentioned before, the is_label_error will place a \0 in the start if the label is invalid! */
            /* Move the line pointer past the label */
            p_line += strlen(label) + 1;
        }

        if (strstr(p_line, ".entry") != NULL) {
            p_line = strstr(p_line, ".entry"); /* Track the .entry command */
            handle_entry_line(p_line, labels, line_num, &found_error);
            continue;
        } 


        /* Skip any spaces before the acutal instruction */
        skip_spaces(&p_line);

        operation_name = get_op_name(p_line); /* Get the operation name */

        /* Skip the operation name */
        p_line += strlen(operation_name);

        /* Skip any spaces after the operation name & before the operandss */
        skip_spaces(&p_line);

        /* Find the operands and put them in the array */
        operands = (char **) safe_malloc(sizeof(char *) * MAX_OPERANDS);
        operands_count = 0;

        /* We don't want to repeat the same error code so if theres a problem faced in first pass we just skip the line*/

        if (!validate_operand_list(p_line, line_num, 0)) {
            found_error = 1;
            continue;
        }

        get_operands(p_line, operands, &operands_count, line_num);

        find_address_modes(operands, operands_count, &dest, &source);

        ic_counter += 1;

        is_source_reg = source == REGISTER || source == POINTER;
        is_dest_reg = dest == REGISTER || dest == POINTER;

        /* Note that we're only trying to match the ic_counter as we did in the first pass so once we would need to replace a label, 
            we would know where it's placed at the code array of the machine. 
        */

        if (is_source_reg && is_dest_reg) {
            ic_counter += 1; /* Destination and source are encoded at the same word */
            free_operands(operands, operands_count);
            safe_free(operation_name);
            continue; 
        } 
        
        if (first_pass_error) {
            /* If errors were already found during first pass there's no point of trying to rebuild words */
            free_operands(operands, operands_count);
            safe_free(operation_name);
            continue;
        }

        if (operands_count == 1) { 
            /* If the dest (single operand) is DIRECT (label), attempt a r eplacement */
            if (dest == DIRECT && !replace_label(operands[0], labels, extern_usage, code_image, ic_counter, line_num)) { 
                found_error = 1;
            }
            ic_counter += 1;
        } 
        
        if (operands_count == 2) {
            /* Same here, but attempt replacing source & dest */
            if (source == DIRECT && !replace_label(operands[0], labels, extern_usage, code_image, ic_counter, line_num)) {
                found_error = 1;
            }
            ic_counter += 1;

            if (dest == DIRECT && !replace_label(operands[1], labels, extern_usage, code_image, ic_counter, line_num)) {
                found_error = 1;
            } 
            ic_counter += 1;
        }

        /* Free all operands */
        free_operands(operands, operands_count);
        safe_free(operation_name);
    }

    /* Close the file we've opened */
    fclose(input_file);
    
    if (found_error) {
        return FOUND_ERROR;
    }

    return NO_PASS_ERROR;
}