#include "first_pass.h"


void update_labels(Labels *labels, int ic)
{
    LabelEntry *current = labels->head;

    while (current != NULL)
    {
        if (current->type == DATA_LABEL)
        {
            current->value += ic;
        }
        current = current->next;
    }
}


PassError first_pass(char *file_name, int *ic, int *dc, Labels *labels, machine_word **code_image, machine_word **data_image)
{

    FILE *input_file = open_file(file_name, "r");

    char *instructions[] = {
        ".data",
        ".string",
        ".entry",
        ".extern"
    };

    int line_num = 0;
    int found_line_type = 0;
    int i = 0;
    int found_error = 0;

    char line[MAX_LINE_SIZE + 2];
    char *p_line;

    log_info("Intialized first pass in file %s\n", file_name);

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL)
    {
        line_num++;
        p_line = line; 
        found_line_type = 0;


        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n')
        {
            continue;
        }


        for (i = 0; i < sizeof(instructions) / sizeof(char *); i++)
        {
            if (strstr(p_line, instructions[i]) != NULL)
            {
                /* Instruction aka .data, .string, .entry, .extern */
                if (!handle_data_line(p_line, line_num, ic, dc, labels, data_image)) {
                    found_error = 1;
                }
                found_line_type = 1;
                break;
            }
        }

        if (found_line_type == 0)
        {
            /* aka mov, add, stop operations, etc */
            if (handle_code_line(p_line, line_num, ic, labels, code_image))
            {
                found_error = 1;
            }
        }
        

    }

    if (found_error)
    {
        return FOUND_ERROR;
    } 

    /* We update the labels with the ic value */

    update_labels(labels, *ic);

    close_file(input_file);
    

    return NO_PASS_ERROR;
    

}