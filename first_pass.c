#include "first_pass.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "definitions.h"

#include "syntax/helpers.h"

char* is_label(char *p) {
    char *p_label = NULL;

    
    int i = 0;
    int j = 0;
    p_label = safe_malloc(strlen(p));

    while (p[i] != '\0' && p[i] != EOF && p[i] != '\n' && p[i] != ':')
    {
        p_label[j] = p[i];
        i++;
        j++;
    }
    if (*p == ':')
    {
        p_label[j] = '\0';
        p_label = safe_realloc(p_label, strlen(p_label));
        return p_label;
    }
    return NULL;

}

int first_pass(char *p_fileName, long *p_ic, long *p_dc, ht_t *p_data_image, ht_t *p_code_image)
{
    FILE *p_file = NULL;

    int lineCount = 0;
    char line[MAX_LINE_SIZE];
    char *p_line = NULL;

    log_info("Intialized first pass in file %s\n", p_fileName);

    p_file = open_file(p_fileName, "r");

    while (fgets(line, MAX_LINE_SIZE, p_file) != NULL)
    {
        /* TODO: Check if the line is too long */

        lineCount++;
        p_line = line;

        /* Skip any spaces at the beginning of the line */
        skip_spaces(&p_line);
        
        /* skip blank line */
        if (*p_line == '\0')
        {
            continue;
        }

        /*
        char *p_label = is_label(p_line);

        if (p_label != NULL)
        {
            log_info("Found label: %s\n", p_label);
            safe_free(p_label);
        }
*/

        

    }



    close_file(p_file);

    return 0;
    

}