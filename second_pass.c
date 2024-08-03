#include "second_pass.h"

PassError second_pass(char *file_name, int *ic, int *dc, Labels *labels, List *macros, machine_word **code_image, machine_word **data_image) {
    
    FILE *input_file = open_file(file_name, "r");
    
    int line_num = 0;
    int found_error = 0;
    char line[MAX_LINE_SIZE + 2];
    char *p_line;
    log_info("Intialized second pass in file %s\n", file_name);

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL) {
        line_num++;
        p_line = line;
        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n') {
            continue;
        }

        if (strstr(p_line, ".entry") != NULL) {
            /* Check that the label is not already defined as an extern */
            
            p_line += strlen(".entry");
            skip_spaces(&p_line);

            char *label = (char *) safe_malloc(MAX_LABEL_SIZE);
            copy_string_until_space(label, p_line);

            LabelEntry *entry = labels_get(labels, label);

            if (entry != NULL) {
                if (entry->type == EXTERN_LABEL) {
                    log_error("Label already defined as extern in line %d\n\tLabel: %s\n", line_num, label);
                    safe_free(label);
                    found_error = 1;
                    continue;
                } else if (entry->type == ENTRY_LABEL) {
                    log_error("Label already defined as entry in line %d\n\tLabel: %s\n", line_num, label);
                    safe_free(label);
                    found_error = 1;
                    continue;
                }
                labels_insert(labels, label, entry->value, ENTRY_LABEL);
            } 


            safe_free(label);  
        } 

        if (strstr(p_line, ".extern") != NULL || strstr(p_line, ".data") != NULL || strstr(p_line, ".string") != NULL) {
            continue;
        }

        char **operands = (char **) safe_malloc(sizeof(char *) * MAX_OPERANDS);
        int operands_count = 0;

        get_operands(p_line, operands, &operands_count);

        int i; 
        for (i= 0; i < operands_count; i++) {
            if (is_label(operands[i])) {
                LabelEntry *entry = labels_get(labels, operands[i]);
                if (entry != NULL) {
                    printf("In line %d, operand: %s to be replaced with address %d\n", line_num, operands[i], entry->value);
                }
            }
        }
        
    }

    debug_labels(labels);

    fclose(input_file);
    
    if (found_error) {
        return FOUND_ERROR;
    }

    return NO_PASS_ERROR;
}