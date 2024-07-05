#include <stdio.h>
#include <stdlib.h>

#include "syntax/helpers.h"
#include "syntax/symbols.h"
#include "macros.h"

int main(int argc, char *argv[])
{

    printf("----------------------\n");

    int i;
    MacroResult *res = NULL;
    if (argc < 2)
    {
        log_error("No files provided\n\tUsage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        exit(1);
    }
    symbols_init();
    log_info("Searching for macros in %d files\n", argc-1);
    for (i = 1; i < argc; i++)
    {
        /*TODO: when finding error in a file, skip the file*/
        res = replace_macros(argv[i]);
        if (res->error == NO_ERROR)
        {
            log_info("File %s processed successfully\n", argv[i]);
        }
        else
        {
            log_error("Error in file (Skipping) %s: with code: %d\n", argv[i], res->error);
        }
        safe_free(res);
    }
    log_info("Finished. \n");

    printf("----------------------\n");

    symbols_free();
    return 0;
}