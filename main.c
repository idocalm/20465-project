#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "macros.h"
#include "first_pass.h"
#include "globals.h"

#define INITIAL_IC_VALUE 100

int process_file(char *p_fileName) {

    int ic = INITIAL_IC_VALUE;
    int dc = 0;

    ht_t *p_labels = ht_create();
    ht_t *p_macros = ht_create();

    

    int res = handle_macros(p_fileName, p_macros);
    if (res != NO_MACRO_ERROR)
    {
        log_error("Error in file during pre-processor (Skipping) %s: with code: %d\n", p_fileName, res);
        return res;
    }

    log_success("Pre-processor finished successfully for file %s\n", p_fileName);

    
    first_pass(p_fileName, &ic, &dc, p_labels,  p_macros);

    

    return 0;
}



int main(int argc, char *argv[])
{
    
    int i;
    char **pp_files = (char **)safe_malloc(sizeof(char *) * argc);
 
    printf("----------------------\n");

    if (argc < 2)
    {
        log_error("No files provided\n\tUsage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        exit(1);
    }


    /* create an array containg all the argv's with .as after the name */

    for (i = 1; i < argc; i++)
    {
        pp_files[i] = (char *)safe_malloc(strlen(argv[i]) + 4);
        strcpy(pp_files[i], argv[i]);
        strcat(pp_files[i], ".as");
    }


    log_info("Received %d files to process\n", argc  -1);
    for (i = 1; i < argc; i++)
    {
        process_file(pp_files[i]);
    }

    /* free the memory */
    for (i = 1; i < argc; i++)
    {
        safe_free(pp_files[i]);
    }
    safe_free(pp_files);


    log_info("Task finished. \n");

    printf("----------------------\n");

    return 0;
}
