#include "data_line.h"

int insert_data_arguments(char *line, int *dc, machine_word **data_image, int line_num) {
    char *operand = NULL;
    char *ptr = line;
    char *last_char = line + strlen(line) - 1;
    int value; 


    if (line[0] == ',') {
        log_error("Invalid data in line %d.\n\t Data starts with a comma\n", line_num);
        return 0;
    }

    /* Validate that the line doesn't end with a comma */
    while (isspace(*last_char)) {
        last_char--;
    }

    if (*last_char == ',') {
        log_error("Invalid data in line %d.\n\t Data ends with a comma\n", line_num);
        return 0;
    }

    /* Check for no double commas */
    while ((operand = strchr(ptr, ',')) != NULL) {
        operand++; 
        skip_spaces(&operand);
        if (*operand == ',') {
            log_error("Invalid data in line %d.\n\t Double commas in data\n", line_num);
            return 0;
        }
        ptr = operand + 1;
    }   

    ptr = line;

    while ((operand = strtok(ptr, ",")) != NULL) {
    
        remove_all_spaces(operand);
        value = is_integer(operand);

        if (value == NON_VALID_INTEGER) {
            log_error("Invalid data in line %d\n\t. Data '%s' is not an integer\n", line_num,  operand);
            return 0;
        }


        data_image[*dc] = (machine_word *) safe_malloc(sizeof(machine_word));
        data_image[*dc]->data = value;
        (*dc)++;

        ptr = NULL;
    }

    return 1;

}

void insert_string_arguments(char *line, int *dc, machine_word **data_image, int line_num) {
    char *ptr = line; 
    while (*ptr != '\0' && *ptr != '\n' && *ptr != '"') {
        data_image[*dc] = (machine_word *) safe_malloc(sizeof(machine_word));
        data_image[*dc]->data = *ptr;
        (*dc)++;
        ptr++;
    }

    data_image[*dc] = (machine_word *) safe_malloc(sizeof(machine_word));
    data_image[*dc]->data = '\0';
    (*dc)++;
}

int insert_extern_arguments(char *line, Labels *labels, int line_num) {
    char *entry_label = (char *) safe_malloc(MAX_LABEL_SIZE);   
    copy_string_until_space(entry_label, line);
    entry_label = (char *) safe_realloc(entry_label, strlen(entry_label) + 1);
    entry_label[strlen(entry_label)] = '\0';

    if (!is_label(entry_label)) {
        log_error("Invalid entry in line %d\n\tEntry label is not a valid label\n", line_num);
        return 0;
    }

    /* TODO: Can you assume that the label is not already defined? */

    labels_insert(labels, entry_label, 0, EXTERN_LABEL);
    safe_free(entry_label);
    return 1;

}

int handle_data_line(char *line, int line_num, int *ic, int *dc, Labels *labels, machine_word **data_image) {

    Instruction instruction;
    char *instruction_name = NULL;
    int found_error = 0;
    char *ptr = NULL;
    char *label = (char *) safe_malloc(MAX_LABEL_SIZE);

    skip_spaces(&line);

    /* #1 - Extract the label, if exists. */
    
    if (is_label_error(line, line_num, label, 1)) {
        found_error = 1;
        safe_free(label);
        return 0; /* Maybe this shouldn't be a return */
    }


    if (label[0] != '\0') {
        line += strlen(label) + 1;
    }

    skip_spaces(&line);

    instruction_name = (char *) safe_malloc(MAX_LINE_SIZE);
    copy_string_until_space(instruction_name, line);

    instruction = get_instruction(instruction_name);

    if (instruction != ENTRY && instruction != EXTERN && label[0] != '\0') { 
        LabelEntry *entry = labels_get(labels, label, DATA_LABEL);
        if (entry != NULL) {
            log_error("Label already defined in line %d\n\tLabel: %s\n", line_num, label);
            found_error = 1;
        } else {
            labels_insert(labels, label, *dc, DATA_LABEL);
        }
    } else if ((instruction == ENTRY || instruction == EXTERN) && label[0] != '\0') { 
        log_warning("Instruction %s in line %d doesn't support defining a label beforehand ('%s')\n", instruction_name, line_num, label);
    }

    safe_free(label);

    line += strlen(instruction_name);
    
    skip_spaces(&line);

    if (instruction == STRING) {
        if (*line != '"') {
            log_error("Invalid string in line %d\n\tExpected: \"<string>\", got: %s\n", line_num, line);
            return 0;
        }

        line++;
        ptr = line + 1;
        while (!isspace(*ptr) && *ptr != '"') {
            ptr++;
        } 

        if (*ptr != '"') {
            log_error("Invalid string in line %d\n\tExpected: \"<string>\", got: %s\n", line_num, line);
            return 0;
        }

        ptr++; /* Skip the " */

        skip_spaces(&ptr);

        if (*ptr != '\0' && *ptr != '\n') {
            log_error("Invalid string in line %d\n\tToo many arguments in string\n", line_num);
            return 0;
        }

        insert_string_arguments(line, dc, data_image, line_num);

    } else if (instruction == DATA) {
        if (*line == ',') {
            log_error("Invalid data in line %d\n\tExpected: <number>, got: %s\n", line_num, line);
            return 0;
        }

        if (!insert_data_arguments(line, dc, data_image, line_num)) {
            found_error = 1;
        }

    } else if (instruction == EXTERN) {
        if (!insert_extern_arguments(line, labels, line_num)) {
            found_error = 1;
        }
        
    }

    safe_free(instruction_name);

    if (found_error) {
        return 0;
    }

    return 1;

}