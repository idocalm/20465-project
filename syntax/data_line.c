#include "data_line.h"

void insert_data_arguments(char *line, int *dc, machine_word **data_image, int line_num) {
    char *operand = NULL;
    char *ptr = line;
    char *last_char = line + strlen(line) - 1;
    int value; 


    if (line[0] == ',') {
        log_error("Invalid data in line %d.\n\t Data starts with a comma\n", line_num);
        return;
    }

    /* Validate that the line doesn't end with a comma */
    while (isspace(*last_char)) {
        last_char--;
    }

    if (*last_char == ',') {
        log_error("Invalid data in line %d.\n\t Data ends with a comma\n", line_num);
        return;
    }


    /* Check for no double commas */
    while ((operand = strchr(ptr, ',')) != NULL) {
        operand++; 
        skip_spaces(&operand);
        if (*operand == ',') {
            log_error("Invalid data in line %d.\n\t Double commas in data\n", line_num);
            return;
        }
        ptr = operand + 1;
    }   

    ptr = line;

    while ((operand = strtok(ptr, ",")) != NULL) {
        
        remove_all_spaces(operand);
        value = is_integer(operand);

        if (value == NON_VALID_INTEGER) {
            log_error("Invalid data in line %d\n\t. Data '%s' is not an integer\n", line_num,  operand);
            return;
        }

        data_image[*dc] = (machine_word *) safe_malloc(sizeof(machine_word));
        data_image[*dc]->data = value;
        data_image[*dc]->ic = *dc;
        (*dc)++;
        ptr = NULL;
    }


}

void insert_string_arguments(char *line, int *dc, machine_word **data_image, int line_num) {
    char *ptr = line + 1;
    while (*ptr != '\0' && *ptr != '\n' && *ptr != '"') {
        data_image[*dc] = (machine_word *) safe_malloc(sizeof(machine_word));
        data_image[*dc]->data = *ptr;
        data_image[*dc]->ic = *dc;
        (*dc)++;
        ptr++;
    }

    data_image[*dc] = (machine_word *) safe_malloc(sizeof(machine_word));
    data_image[*dc]->data = '\0';
    data_image[*dc]->ic = *dc;
    (*dc)++;
}

void insert_extern_arguments(char *line, Labels *labels, int line_num) {
    char *entry_label = (char *) safe_malloc(MAX_LABEL_SIZE);   
    copy_string_until_space(entry_label, line);
    entry_label = (char *) safe_realloc(entry_label, strlen(entry_label) + 1);
    entry_label[strlen(entry_label)] = '\0';

    if (!is_label(entry_label)) {
        log_error("Invalid entry in line %d\n\tEntry label is not a valid label\n", line_num);
        return;
    }

    /* Can you assume that the label is not already defined? */

    labels_insert(labels, entry_label, 0, EXTERN_LABEL);

}

void handle_instruction_line(char *line, int line_num, int *ic, int *dc, Labels *labels, List *macros, machine_word **data_image) {

    char *label = NULL;
    Directive directive;
    char *directive_name = NULL;
    char *ptr = NULL;

    skip_spaces(&line);

    /* #1 - Extract the label, if exists. */
    if ((label = is_label_def(line, macros)) != NULL) {
        line += strlen(label) + 1;
    }

    skip_spaces(&line);

    directive_name = (char *) safe_malloc(MAX_LINE_SIZE);
    copy_string_until_space(directive_name, line);

    directive = get_directive(directive_name);

    if (directive != ENTRY && directive != EXTERN && label != NULL) {
        LabelEntry *entry = labels_get(labels, label);
        if (entry != NULL && entry->type == DATA_LABEL) {
            log_error("Label already defined in line %d\n\tLabel: %s\n", line_num, label);
            safe_free(label);
            return;
        }
        labels_insert(labels, label, *dc, DATA_LABEL);
        (*dc)++;
        safe_free(label);
    } else if ((directive == ENTRY || directive == EXTERN) && label != NULL) {
        log_warning("Directive %s in line %d doesn't support defining a label ('%s')\n", directive_name, line_num, label);
    }

    line += strlen(directive_name);
    
    skip_spaces(&line);

    if (directive == STRING) {
        if (*line != '"') {
            log_error("Invalid string in line %d\n\tExpected: \"<string>\", got: %s\n", line_num, line);
            return;
        }

        line++;
        ptr = line + 1;
        while (!isspace(*ptr) && *ptr != '"') {
            ptr++;
        } 

        if (*ptr != '"') {
            log_error("Invalid string in line %d\n\tExpected: \"<string>\", got: %s\n", line_num, line);
            return;
        }

        insert_string_arguments(line, dc, data_image, line_num);

    } else if (directive == DATA) {
        if (*line == ',') {
            log_error("Invalid data in line %d\n\tExpected: <number>, got: %s\n", line_num, line);
            return;
        }

        insert_data_arguments(line, dc, data_image, line_num);
    } else if (directive == EXTERN) {
        insert_extern_arguments(line, labels, line_num);
    }

}