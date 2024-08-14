#include "first_pass.h"

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

    char label[MAX_LINE_SIZE + 1]; /* The label buffer */

    int line_num = 0; /* The current line number */
    int found_error = 0; /* A flag to see if an error was found */

    LineType line_type; /* The type of the line */

    char line[MAX_LINE_SIZE + 2]; /* line buffer */
    char *p_line;  /* Pointer to the line (we don't want to lose the line itself )*/

    label[MAX_LINE_SIZE] = '\0'; /* Null terminate the label */
    log_info("Intialized first pass in file %s\n", file_name);

    while (fgets(line, MAX_LINE_SIZE + 2, input_file) != NULL)
    {
        line_num++;
        p_line = line; 

        /* Skip any unimportant spaces */
        skip_spaces(&p_line);

        if (is_label_error(p_line, line_num, label, 1))  /* Check if the label is valid (doesn't exists / exists & valid) */
        {
            /* A note: once the label has some sort of error we can't cotinue 
                because we don't know how to analyze the sentence
                So we skip to the next lien with an error 
            */
            found_error = 1;
            continue;
        }

        if (label[0] != '\0') /* The is_label_error function sets first char to '\0' if there is no label */
        {
            char *search = p_line + strlen(label) + 1;
            remove_all_spaces(search);
            if (strlen(search) == 0) /* If the label is the only thing in the line */
            {
                log_line_error(line_num, line, "Label '%s' has no content.", label);
                found_error = 1;
                continue;
            }
            /* Move the line pointer past the label */
            p_line += strlen(label) + 1;
        }

        line_type = get_line_type(p_line); 

        /* Skip any spaces before the acutal instruction */
        skip_spaces(&p_line);

        if (line_type == LINE_DATA)
        {
            if (handle_data_line(p_line, line_num, dc, label, labels, data_image))
            {
                found_error = 1;
            }
        }
        else if (line_type == LINE_CODE)
        {
            if (handle_code_line(p_line, line_num, ic, label, labels, code_image))
            {
                found_error = 1;
            }
        }

    }

    close_file(input_file); 

    if (found_error)
        return FOUND_ERROR; /* The first pass has FAILED! */

    /* We update the labels with the ic value */
    update_labels(labels, *ic);

    return NO_PASS_ERROR; /* The first pass has finished successfully!! */
}