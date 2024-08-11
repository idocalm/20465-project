#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "helpers/memory.h"
#include "file_handler.h"

/**
    * @brief main function of the assembler (loop through ALL files)
    * @param argc The number of arguments
    * @param argv arguments array
*/

int main(int argc, char *argv[])
{
    
    int i;
    char **files = (char **)safe_malloc(sizeof(char *) * argc);
 
    printf("--- An Assembler written in C ---\n");
    printf(BOLD"--- Made by: Ido Calman & Roey Faiman ---\n"RESET);
    printf(BOLD"--- Semester: 2024B ---\n"RESET);

    if (argc < 2)
    {
        log_error("No files were provided to the assembler.\n\tUsage: %s <file1> <file2> ... <fileN>.\n", argv[0]);
        return 1;
    }

    /* create an array containg all the argv's with .as after the name */

    for (i = 1; i < argc; i++)
    {
        files[i] = (char *) safe_malloc(strlen(argv[i]) + 4);
        strcpy(files[i], argv[i]);
        strcat(files[i], ".as");
    }


    log_info("Received %d files to process\n", argc - 1);
    for (i = 1; i < argc; i++)
    {
        handle_file(files[i]);
    }

    /* free the memory */
    for (i = 1; i < argc; i++)
    {
        safe_free(files[i]);
    }
    safe_free(files);

    printf("----------------------\n");

    return 0;
}
