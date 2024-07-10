#include <stdio.h>
#include <stdlib.h>

#include "syntax/helpers.h"
#include "syntax/symbols.h"
#include "macros.h"
#include "first_pass.h"

#define INITIAL_IC_VALUE 100

int process_file(char *p_fileName) {

    long ic = INITIAL_IC_VALUE;
    long dc = 0;

    ht_t *p_data_image = ht_create();
    ht_t *p_code_image = ht_create();
    

    int res = handle_macros(p_fileName);
    if (res != NO_ERROR)
    {
        log_error("Error in file during pre-processor (Skipping) %s: with code: %d\n", p_fileName, res);
        return res;
    }

    first_pass(p_fileName, &ic, &dc, p_data_image, p_code_image);

    

    return 0;
}



int main(int argc, char *argv[])
{
    int i;
     
    printf("----------------------\n");

    if (argc < 2)
    {
        log_error("No files provided\n\tUsage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        exit(1);
    }

    log_info("Searching for macros in %d files\n", argc-1);
    for (i = 1; i < argc; i++)
    {
        process_file(argv[i]);
    }

    log_info("Task finished. \n");

    printf("----------------------\n");

    return 0;
}
