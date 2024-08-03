#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "macros.h"
#include "first_pass.h"
#include "second_pass.h"
#include "globals.h"
#include "structs/labels.h"

void free_code_image(machine_word **code_image)
{
    size_t size = sizeof(code_image) / sizeof(machine_word);
    int i = 0;
    for (i = 0; i < size; i++)
    {
        safe_free(code_image[i]);
    }
}

void free_data_image(machine_word **data_image)
{
    size_t size = sizeof(data_image) / sizeof(machine_word);
    int i = 0;
    for (i = 0; i < size; i++)
    {
        safe_free(data_image[i]);
    }
}



int handle_file(char *file_name) {

    int ic = INITIAL_IC_VALUE;
    int dc = 0;
    int found_error = 0;
    PassError error = NO_PASS_ERROR;

    Labels *labels = labels_create();
    List *macros = list_create();

    machine_word *code_image[ASSEMBLER_MAX_CAPACITY];
    machine_word *data_image[ASSEMBLER_MAX_CAPACITY];

    int res = handle_macros(file_name, macros);
    if (res != NO_MACRO_ERROR)
    {
        log_error("Error in file during pre-processor (Skipping) %s: with code: %d\n", file_name, res);
        return res;
    }

    log_success("Pre-processor finished successfully for file %s\n", file_name);

    /* Change the file to the .am file */
    file_name[strlen(file_name) - 1] = 'm';

    /* Start first pass */

    error = first_pass(file_name, &ic, &dc, labels, macros, code_image, data_image);
    if (error != NO_PASS_ERROR)
    {
        log_error("Error in file during first pass (Skipping) %s: with code: %d\n", file_name, error);
        found_error = 1;
    }

    log_success("First pass finished successfully for file %s\n", file_name);

    /* Start second pass */


    if (!found_error)
    {
        error = second_pass(file_name, &ic, &dc, labels, macros, code_image, data_image);
        if (error != NO_PASS_ERROR)
        {
            log_error("Error in file during second pass (Skipping) %s: with code: %d\n", file_name, error);
            found_error = 1;
        } else {
            log_success("All passes are finished. No errors found in file %s\n", file_name);
        }
    }


    labels_free(labels);
    list_free(macros);
    free_code_image(code_image);
   /* free_data_image(data_image); */


    return 0;
}



int main(int argc, char *argv[])
{
    
    int i;
    char **files = (char **)safe_malloc(sizeof(char *) * argc);
 
    printf("----------------------\n");

    if (argc < 2)
    {
        log_error("No files provided\n\tUsage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        exit(1);
    }

    /* create an array containg all the argv's with .as after the name */

    for (i = 1; i < argc; i++)
    {
        files[i] = (char *)safe_malloc(strlen(argv[i]) + 4);
        strcpy(files[i], argv[i]);
        strcat(files[i], ".as");
    }


    log_info("Received %d files to process\n", argc  -1);
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
