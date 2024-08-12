#include "file_handler.h"
/**
    * @brief Main function to handle a file through all stages
    * @param file_name The name of the file (without .as)
*/

void handle_file(char *file_name) {

    int ic = INITIAL_IC_VALUE; /* Assigning an instruction counter for this file */
    int dc = 0; /* Assigning a data counter for this file */
    int found_error = 0; /* Flag to indicate if an error was found anywhere */
    char *new_file_name = (char *)safe_malloc(strlen(file_name) + 2); /* The new file name with .am */
    FILE *attempt;
    int i = 0;

    Labels *labels = labels_create(); /* Label table (for first & second pass )*/
    List *extern_usage = list_create(); /* Extern usage (mainly for second pass and .ext output file)*/

    machine_word *code_image[ASSEMBLER_MAX_CAPACITY]; /* Code image of the machine */
    machine_word *data_image[ASSEMBLER_MAX_CAPACITY]; /* Data image of the machine */

    set_file_name(file_name); /* Update the logs.c so we would print the correct name during errors/warnings */

    printf(BOLD"Processing file %s\n"RESET, file_name);
    attempt = fopen(file_name, "r"); /* Open the file with reading perm */
    if (attempt == NULL)
    {
        log_error("Unable to open file '%s'. Skipping to the next file.\n", file_name);
        list_free(extern_usage);
        labels_free(labels);
        safe_free(new_file_name);
        return;
    } 

    fclose(attempt);

    /* STAGE 1: Pre-processor (macro extracting and replacing ) */
    if (handle_macros(file_name) != NO_MACRO_ERROR)
    {
        /* We can't continue after an error in the macro stage because there's no .am file */
        log_error("One or more errors were found in file '%s' during pre-processor. Moving to the next file.\n", file_name);
        list_free(extern_usage);
        labels_free(labels);
        safe_free(new_file_name);

        return; 
    }


    log_success("Pre-processor finished successfully for file '%s'\n", file_name);


    /* Change the file to the .am file */
    for (; i < strlen(file_name) - 1; i++)
    {
        new_file_name[i] = file_name[i];
    }
    new_file_name[i] = 'm';
    new_file_name[i + 1] = '\0';
    set_file_name(new_file_name); /* Set the file name for the logs */

    /* STAGE 2: Start first pass */

    if (first_pass(new_file_name, &ic, &dc, labels, code_image, data_image) == NO_PASS_ERROR) {
        log_success("First pass finished successfully for file %s\n", new_file_name);
    } else {
        found_error = 1;
    }

    /**
    debug_labels(labels);
    */    


    /* STAGE 3: Start second pass */

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

    log_info("Creating output files for %s\n", new_file_name);

    /* STAGE 4: Create the output files */

    create_output_files(new_file_name, labels, extern_usage, code_image, data_image, &ic, &dc);

    /* Free all the memory we used */
    labels_free(labels);
    list_free(extern_usage);
    free_code_image(code_image, ic);
    free_data_image(data_image, dc);

    log_info(BOLD"Finished processing file %s\n"RESET, new_file_name);
    safe_free(new_file_name);

}
