#include "second_pass.h"


int replace_label(char *operand, Labels *labels, List *extern_usage, machine_word **code_image, int ic_counter, int line_num) {
    LabelEntry *entry = labels_get_any(labels, operand);
    machine_word *word = NULL; 

    if (entry == NULL) {
        log_error("Label referenced in line %d is not defined\n\tLabel: %s\n", line_num, operand);
        return 0;
    }

    word = code_image[ic_counter - INITIAL_IC_VALUE];

    if (!word) {
        log_warning("No word found in code image at index %d in line %d \n", ic_counter, line_num);
        return 0; /* Maybe this should be an error */
    }

    if (entry->type == EXTERN_LABEL) {
        /* It's currently all 0. Turn on the E bit (first from right )*/
        code_image[ic_counter - INITIAL_IC_VALUE]->data |= (1 << 0);
        list_insert_integer(extern_usage, entry->key, ic_counter);
    } else {
        /* It's currently all 0. Turn on the R bit (second from right )*/
        code_image[ic_counter - INITIAL_IC_VALUE]->data |= (1 << 1);
    }
    
    code_image[ic_counter - INITIAL_IC_VALUE]->data |= (entry->value << 3);

    return 1;
}

void handle_entry_line(char *line, Labels *labels, int line_num, int *found_error) {

    char *value, *validate;
    LabelEntry *entry;


    line += strlen(".entry");
    skip_spaces(&line);

    value = (char *) safe_malloc(MAX_LABEL_SIZE + 1);
    copy_string_until_space(value, line);

    validate = line + strlen(value) + 1; 

    skip_spaces(&validate);

    if (*validate != '\0' && *validate != '\n') {
        log_error(".entry has more then one operands (externous characters / second label) in line %d\n", line_num);
        safe_free(value);
        *found_error = 1;
        return;
    }

    /* Check that the label is not already defined as an extern */

    if (labels_get(labels, value, EXTERN_LABEL) != NULL) {
        log_error("Label already defined as extern in line %d\n\tLabel: %s\n", line_num, value);
        *found_error = 1;
        safe_free(value);
        return;
    }


    if (labels_get(labels, value, ENTRY_LABEL) != NULL) {
        log_error("Label already defined as entry in line %d\n\tLabel: %s\n", line_num, value);
        *found_error = 1;
        safe_free(value);
        return;
    }


    if (labels_get(labels, value, CODE_LABEL) == NULL && labels_get(labels, value, DATA_LABEL) == NULL) {   
        log_error("Label referenced in .entry is not defined in line: %d\n\tLabel: %s\n", line_num, value);
        *found_error = 1;
        safe_free(value);
        return;
    }



    entry = labels_get_any(labels, value);
    labels_insert(labels, value, entry->value, ENTRY_LABEL);


}

void find_address_modes(char **operands, int operands_count, AddressMode *dest, AddressMode *source) {
    if (operands_count == 1) {
        *dest = address_mode(operands[0], 0, 0, NULL);
        *source = UNKNOWN_ADDRESS;
    } else if (operands_count == 2) {
        *source = address_mode(operands[0], 0, 0, NULL);
        *dest = address_mode(operands[1], 0, 0, NULL);
    } 
}

PassError second_pass(char *file_name, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int first_pass_error) {

    FILE *input_file = open_file(file_name, "r");

    int line_num = 0;
    int found_error = 0;
    char line[MAX_LINE_SIZE + 2];
    char *p_line;
    char *value, *validate;

    int ic_counter = INITIAL_IC_VALUE;
    char *operation_name;
    LabelEntry *entry;
    int is_dest_reg, is_source_reg;

    char **operands;
    int operands_count;
    AddressMode dest, source;
    
    char label[MAX_LABEL_SIZE + 1];
    label[MAX_LABEL_SIZE] = '\0';

    log_info("Intialized second pass in file %s\n", file_name);

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) {
        line_num++;
        p_line = line;

        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n') { /* Empty lines */
            continue;
        }

        if (strstr(p_line, ".extern") != NULL || strstr(p_line, ".data") != NULL || strstr(p_line, ".string") != NULL) { /* Already handled in first pass */
            continue;
        }

        if (is_label_error(p_line, line_num, label, 0)) {
            found_error = 1;
            continue;
        }

        if (label[0] != '\0') {

            p_line += strlen(label) + 1;

        }

        skip_spaces(&p_line);

        if (strstr(p_line, ".entry") != NULL) {
            p_line = strstr(p_line, ".entry");
            handle_entry_line(p_line, labels, line_num, &found_error);
            continue;
        } 

        operation_name = get_op_name(p_line);

        p_line += strlen(operation_name);

        skip_spaces(&p_line);

        operands = (char **) safe_malloc(sizeof(char *) * MAX_OPERANDS);
        operands_count = 0;

        get_operands(p_line, operands, &operands_count);

        find_address_modes(operands, operands_count, &dest, &source);

        ic_counter += 1;

        is_source_reg = source == REGISTER || source == POINTER;
        is_dest_reg = dest == REGISTER || dest == POINTER;

        if (is_source_reg && is_dest_reg) {
            ic_counter += 1;
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
            if (dest == DIRECT && !replace_label(operands[0], labels, extern_usage, code_image, ic_counter, line_num)) { 
                found_error = 1;
            }
            ic_counter += 1;
        } else if (operands_count == 2) {
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


    fclose(input_file);
    
    if (found_error) {
        return FOUND_ERROR;
    }

    return NO_PASS_ERROR;
}