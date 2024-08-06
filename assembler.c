#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "macros.h"
#include "first_pass.h"
#include "second_pass.h"
#include "output.h"
#include "globals.h"
#include "structs/labels.h"
void free_code_image(machine_word **code_image, int *ic)
{
    
    int i = 0;
    for (i = 0; i < *ic - INITIAL_IC_VALUE; i++)
    {
        safe_free(code_image[i]);
    }
}

void free_data_image(machine_word **data_image, int *dc)
{
    int i = 0;
    for (i = 0; i < *dc; i++)
    {
        safe_free(data_image[i]);
    }
}


int handle_file(char *file_name) {

    int ic = INITIAL_IC_VALUE;
    int dc = 0;
    int found_error = 0;
    PassError error = NO_PASS_ERROR;
    size_t len = strlen(file_name);

    char new_file_name[len + 2];

    Labels *labels = labels_create();
    List *macros = list_create();
    List *extern_usage = list_create();

    machine_word *code_image[ASSEMBLER_MAX_CAPACITY];
    machine_word *data_image[ASSEMBLER_MAX_CAPACITY];

    int res = handle_macros(file_name, macros);
    if (res != NO_MACRO_ERROR)
    {
        log_error("Error in file during pre-processor (Skipping) %s: with code: %d\n", file_name, res);
        return res;
    }

    log_success("Pre-processor finished successfully for file %s\n", file_name);
    list_free(macros);

    /* Change the file to the .am file */
    int i;
    for (i = 0; i < strlen(file_name) - 1; i++)
    {
        new_file_name[i] = file_name[i];
    }
    new_file_name[i] = 'm';
    new_file_name[i+1] = '\0';

    /* Start first pass */

    error = first_pass(new_file_name, &ic, &dc, labels, macros, code_image, data_image);
    if (error == NO_PASS_ERROR) {
        log_success("First pass finished successfully for file %s\n", new_file_name);
    } else {
        found_error = 1;
    }
       

    /* Start second pass */

    error = second_pass(new_file_name, macros, labels, extern_usage, code_image, data_image);
    if (error == NO_PASS_ERROR && !found_error) {
        log_success("Second pass finished successfully for file %s\n", new_file_name);
    } else {
        found_error = 1;
    }

    if (found_error)
    {
        log_error("Unable to process file %s because of errors.\n", new_file_name);
        return 1;
    }

    log_info("Creating output files for %s\n", new_file_name);


    create_output_files(new_file_name, labels, extern_usage, code_image, data_image, &ic, &dc);

    labels_free(labels);
    list_free(extern_usage);
    free_code_image(code_image, &ic);
    free_data_image(data_image, &dc);

    log_success("Finished processing file %s\n", new_file_name);

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
