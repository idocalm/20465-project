#include "second_pass.h"

int replace_label(char *operand, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int ic_counter, int line_num) {
    

    LabelEntry *entry = labels_get(labels, operand);
    machine_word *word = NULL; 
    if (entry == NULL) {
        log_error("Label referenced in line %d is not defined\n\tLabel: %s\n", line_num, operand);
        return 1;
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

    return 0;

}

PassError second_pass(char *file_name, List *macros, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image) {
    
    FILE *input_file = open_file(file_name, "r");
    
    int line_num = 0;
    int found_error = 0;
    char line[MAX_LINE_SIZE + 2];
    char *p_line;

    
    char label[MAX_LABEL_SIZE + 1];
    label[MAX_LABEL_SIZE] = '\0';

    int ic_counter = INITIAL_IC_VALUE;
    char *operation_name;
    LabelEntry *entry;
    int is_dest_reg, is_source_reg;

    char **operands;
    int operands_count;

    AddressMode dest, source;

    log_info("Intialized second pass in file %s\n", file_name);

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) {
        line_num++;
        p_line = line;

        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n') {
            continue;
        }

        if (strstr(p_line, ".extern") != NULL || strstr(p_line, ".data") != NULL || strstr(p_line, ".string") != NULL) {
            continue;
        }

        if (is_label_error(p_line, line_num, label, 0)) {
            found_error = 1;
            continue;
        }


        if (label[0] != '\0') {
            LabelEntry *entry = labels_get(labels, label);
            if (entry != NULL && entry->type == CODE_LABEL) { 
                p_line += strlen(label) + 1;
            }
        }


        skip_spaces(&p_line);

        if (strstr(p_line, ".entry") != NULL) {
            /* Check that the label is not already defined as an extern */
            
            p_line += strlen(".entry");
            skip_spaces(&p_line);

            char *value = (char *) safe_malloc(MAX_LABEL_SIZE + 1);
            copy_string_until_space(value, p_line);


            char *validate = p_line + strlen(value) + 1; 
            skip_spaces(&validate);

            if (*validate != '\0' && *validate != '\n') {
                log_error(".entry has more then one operands (externous characters / second label) in line %d\n", line_num);
                safe_free(value);
                found_error = 1;
                continue;
            }



            entry = labels_get(labels, value);

            if (entry != NULL) {
                if (entry->type == EXTERN_LABEL) {
                    log_error("Label already defined as extern in line %d\n\tLabel: %s\n", line_num, value);
                    found_error = 1;
                } else if (entry->type == ENTRY_LABEL) {
                    log_error("Label already defined as entry in line %d\n\tLabel: %s\n", line_num, value);
                    found_error = 1;
                    continue;
                }
                labels_insert(labels, value, entry->value, ENTRY_LABEL);
            } else {
                log_error("Label referenced in .entry directive is not defined\n\tLabel: %s\n", value);
                found_error = 1;
            }

            safe_free(value);

            continue;
        } 

        operation_name = (char *) safe_malloc(MAX_LINE_SIZE);
        copy_string_until_space(operation_name, p_line);
        operation_name = (char *) safe_realloc(operation_name, strlen(operation_name) + 1);

        p_line += strlen(operation_name);

        skip_spaces(&p_line);

        operands = (char **) safe_malloc(sizeof(char *) * MAX_OPERANDS);
        operands_count = 0;


        get_operands(p_line, operands, &operands_count);

        if (operands_count == 1) {
            dest = find_addressing_mode(operands[0]);
            source = UNKNOWN_ADDRESS;
        } else if (operands_count == 2) {
            source = find_addressing_mode(operands[0]);
            dest = find_addressing_mode(operands[1]);
        } else {
            dest = UNKNOWN_ADDRESS;
            source = UNKNOWN_ADDRESS;
        }

        ic_counter += 1;

        is_source_reg = source == REGISTER || source == RELATIVE;
        is_dest_reg = dest == REGISTER || dest == RELATIVE;

        if (is_source_reg && is_dest_reg) {
            ic_counter += 1;
        } else if (operands_count == 1) {
            if (dest == DIRECT) { 
                if (replace_label(operands[0], labels, extern_usage, code_image, data_image, ic_counter, line_num)) {
                    found_error = 1;
                }
            }
            ic_counter += 1;
        } else if (operands_count == 2) {
            if (source == DIRECT) {
                if (replace_label(operands[0], labels, extern_usage, code_image, data_image, ic_counter, line_num)) {
                    found_error = 1;
                }
            }

            ic_counter += 1;
            if (dest == DIRECT) {
                if (replace_label(operands[1], labels, extern_usage, code_image, data_image, ic_counter, line_num)) {
                    found_error = 1;
                }
            } 

            ic_counter += 1;
        }

        /* Free all operands */
        int i;
        for (i = 0; i < operands_count; i++) {
            safe_free(operands[i]);
        }
        safe_free(operands);
        safe_free(operation_name);

    }


    fclose(input_file);
    
    if (found_error) {
        return FOUND_ERROR;
    }

    return NO_PASS_ERROR;
}