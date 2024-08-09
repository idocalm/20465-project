#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "macros.h"
#include "first_pass.h"
#include "second_pass.h"
#include "output.h"
#include "definitions.h"
#include "structs/labels.h"
#include "helpers/memory.h"

/**
    * @brief A small welcome message from us (:
*/

void welcome_message() {


    printf("  #####                  #      #####    #####   #######  #     #  ######   #        #######  ######  \n");
    printf(" #     #                # #    #     #  #     #  #        ##   ##  #     #  #        #        #     # \n");
    printf(" #                     #   #   #        #        #        # # # #  #     #  #        #        #     # \n");
    printf(" #                    #     #   #####    #####   #####    #  #  #  ######   #        #####    ######  \n");
    printf(" #                    #######        #        #  #        #     #  #     #  #        #        #   #   \n");
    printf(" #     #              #     #  #     #  #     #  #        #     #  #     #  #        #        #    #  \n");
    printf("  #####               #     #   #####    #####   #######  #     #  ######   #######  #######  #     # \n");

    printf(BOLD" ----- Made by: Ido Calman & Roei Faiman -----\n"RESET);
    printf(BOLD" ----- Semester: 2024B -----\n"RESET);
    
}


/**
    * @brief Main function to handle a file through all stages
    * @param file_name The name of the file (without .as)
*/

void handle_file(char *file_name) {

    int ic = INITIAL_IC_VALUE; /* Assigning an instruction counter for this file */
    int dc = 0; /* Assigning a data counter for this file */
    int found_error = 0; /* Flag to indicate if an error was found anywhere */
    size_t len = strlen(file_name); 
    char *new_file_name = (char *)safe_malloc(len + 2); /* The new file name with .am */
    int i = 0;

    Labels *labels = labels_create(); /* Label table (for first & second pass )*/
    List *macros = list_create(); /* Macro list (for pre-processor )*/
    List *extern_usage = list_create(); /* Extern usage (mainly for second pass and .ext output file)*/

    machine_word *code_image[ASSEMBLER_MAX_CAPACITY]; /* Code image of the machine */
    machine_word *data_image[ASSEMBLER_MAX_CAPACITY]; /* Data image of the machine */

    /* STAGE 1: Pre-processor (macro extracting and replacing ) */
    if (handle_macros(file_name, macros) != NO_MACRO_ERROR)
    {
        /* We can't continue after an error in the macro stage because there's no .am file */
        log_error("One or more errors were found in file '%s' during pre-processor. Moving to the next file.\n", file_name);
        list_free(macros);
        list_free(extern_usage);
        labels_free(labels);
        safe_free(new_file_name);

        return; 
    }

    /* We no longer need the macros so we can free it */
    log_success("Pre-processor finished successfully for file '%s'\n", file_name);

    list_free(macros);


    /* Change the file to the .am file */
    for (; i < strlen(file_name) - 1; i++)
    {
        new_file_name[i] = file_name[i];
    }
    new_file_name[i] = 'm';
    new_file_name[i + 1] = '\0';

    /* Start first pass */

    if (first_pass(new_file_name, &ic, &dc, labels, code_image, data_image) == NO_PASS_ERROR) {
        log_success("First pass finished successfully for file %s\n", new_file_name);
    } else {
        found_error = 1;
    }
       
    /* Start second pass */

    if ((second_pass(new_file_name, labels, extern_usage, code_image, data_image, found_error) == NO_PASS_ERROR) && !found_error) {
        log_success("Second pass finished successfully for file %s\n", new_file_name);
    } else {
        found_error = 1;
    }


    if (found_error)
    {
        log_error("Unable to process file %s because of one or more errors.\n", new_file_name);
        labels_free(labels);
        safe_free(new_file_name);
        list_free(extern_usage);
        free_code_image(code_image, ic);
        free_data_image(data_image, dc);
        return; 
    }

    debug_labels(labels);

    log_info("Creating output files for %s\n", new_file_name);

    create_output_files(new_file_name, labels, extern_usage, code_image, data_image, &ic, &dc);

    /* Free the memory */
    labels_free(labels);
    list_free(extern_usage);
    free_code_image(code_image, ic);
    free_data_image(data_image, dc);

    log_success("Finished processing file %s\n", new_file_name);
    safe_free(new_file_name);

}


/**
    * @brief main function of the assembler (loop through ALL files)
    * @param argc The number of arguments
    * @param argv arguments array
*/
int main(int argc, char *argv[])
{
    
    int i;
    char **files = (char **)safe_malloc(sizeof(char *) * argc);
 
    printf("----------------------\n");

    welcome_message();

    printf("----------------------\n");

    if (argc < 2)
    {
        log_error("No files were provided to the assembler.\n\tUsage: %s <file1> <file2> ... <fileN>.\n", argv[0]);
        exit(EXIT_FAILURE);
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
