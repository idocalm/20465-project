#include "first_pass.h"

FPassErrors first_pass(char *p_fileName, int *p_ic, int *p_dc, List *p_labels, List *p_macros)
{

    FILE *p_file = open_file(p_fileName, "r");

    char *pp_instructions[] = {
        ".data",
        ".string",
        ".entry",
        ".extern"
    };
    int lineNum = 0;
    int i = 0;
    int found_line_type = 0;

    char line[MAX_LINE_SIZE + 2];
    char *p_line;

    log_info("Intialized first pass in file %s\n", p_fileName);

    while (fgets(line, MAX_LINE_SIZE + 2, p_file) != NULL)
    {
        lineNum++;
        p_line = line; 
        found_line_type = 0;
        skip_spaces(&p_line);

        if (*p_line == '\0' || *p_line == '\n')
        {
            continue;
        }


        for (i = 0; i < sizeof(pp_instructions) / sizeof(char *); i++)
        {
            if (strstr(p_line, pp_instructions[i]) != NULL)
            {
                handle_instruction_line(p_line);
                found_line_type = 1;
                break;
            }
        }

        if (found_line_type == 0)
        {
            handle_directive_line(p_line, lineNum, p_ic, p_labels, p_macros, 1); 
        }
        

    }


    close_file(p_file);
    

    return 0;
    

}