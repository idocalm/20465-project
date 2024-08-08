#include "first_pass.h"

/**
    * @brief This function adds the ic value to any DATA_LABEL labels 
    * @param labels The labels struct

    * Note: this is done to create different "zones" int the system memory, one for the code and one for the data! 
    * Information stated at page 53 of the booklet

*/
void update_labels(Labels *labels, int ic)
{
    LabelEntry *current = labels->head;

    while (current != NULL)
    {
        /* Check that the label is indeed a DATA_LABEL before updating the value */
        if (current->type == DATA_LABEL)
        {
            current->value += ic;
        }
        current = current->next;
    }
}

/**
    * @brief The main function of the first pass  
    * @param file_name The name of the file (.am) 
    * @param ic The instruction counter
    * @param dc The data counter
    * @param labels This file's labels struct
    * @param code_image The code image
    * @param data_image The data image
    * @return NO_PASS_ERROR if no error was found, or FOUND_ERROR if an error was found. 
*/
PassError first_pass(char *file_name, int *ic, int *dc, Labels *labels, machine_word **code_image, machine_word **data_image)
{

    FILE *input_file = open_file(file_name, "r"); /* Open the file with reading perm */

    char *instructions[] = { /* list of instruction to look for */
        ".data",
        ".string",
        ".entry",
        ".extern"
    };
    size_t instructions_size = sizeof(instructions) / sizeof(char *);

    int line_num = 0; /* The current line number */
    int found_line_type = 0; /* used to check if the line is a code / data line */
    int i;
    int found_error = 0; /* A flag to see if an error was found */

    char line[MAX_LINE_SIZE + 2]; /* line buffer */

    char *p_line; 

    log_info("Intialized first pass in file %s\n", file_name);

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL)
    {
        line_num++;
        p_line = line; 
        found_line_type = 0; /* Reset the flag */

        /* Skip any unimportant spaces */
        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n')
        {
            /* Empty lines */
            continue;
        }


        for (i = 0; i < instructions_size; i++)
        {
            /* Check for match with an instruction in the line */
            if (strstr(p_line, instructions[i]) != NULL)
            {
                /* Instruction aka .data, .string, .entry, .extern */
                if (handle_data_line(p_line, line_num, ic, dc, labels, data_image)) {
                    found_error = 1; 
                }
                found_line_type = 1;
                break;
            }
        }

        if (found_line_type == 0) /* Hence, the line is a code line */
        {
            /* aka mov, add, stop operations, etc */
            if (handle_code_line(p_line, line_num, ic, labels, code_image))
            {
                found_error = 1;
            }
        }
        

    }

    close_file(input_file); 

    if (found_error)
    {
        return FOUND_ERROR; /* The first pass has FAILED! */
    } 

    /* We update the labels with the ic value */
    update_labels(labels, *ic);


    return NO_PASS_ERROR; /* The first pass has finished successfully!! */
}