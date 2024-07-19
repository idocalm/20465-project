#include "first_pass.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "definitions.h"

#include "syntax/commands.h"
#include "syntax/helpers.h"


FPassErrors first_pass(char *p_fileName, long *p_ic, long *p_dc, ht_t *p_data_image, ht_t *p_code_image, ht_t *p_macros)
{


    char *p_currentLine = (char *)safe_malloc(MAX_LINE_SIZE);
    char *p_line = p_currentLine; 
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


    log_info("Intialized first pass in file %s\n", p_fileName);




    while (fgets(p_currentLine, MAX_LINE_SIZE, p_file) != NULL)
    {
        lineNum++;
        p_line = p_currentLine;
        found_line_type = 0;
        skip_spaces(&p_line);

        /* check if the line is empty */
        if (*p_line == '\0' || *p_line == '\n')
        {
            continue;
        }


        /* check if the line is a instruction */
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
            handle_directive_line(p_line, lineNum, p_macros);
        }


    }

    safe_free(p_currentLine);
    close_file(p_file);
    

    return 0;
    

}