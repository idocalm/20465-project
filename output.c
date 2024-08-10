#include "output.h"

/**
 * @brief Checks if an entry exists in the labels table 
 * @param labels The labels struct
 * @return 1 if an entry exists, 0 otherwise
*/
int does_entry_exist(Labels *labels) {
    LabelEntry *current = labels->head;

    while (current != NULL)
    {
        if (current->type == ENTRY_LABEL)
        {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

/**
    * @brief Checks if there is any usage of an extern label
    * @param extern_usage the extern usage list maintained through the second pass 
    * @return 1 if an extern exists, 0 otherwise
*/

int does_extern_exist(List *extern_usage) {
    Node *current = extern_usage->head;

    while (current != NULL)
    {
        if (current->data != NULL)
        {
            return 1;
        }
        current = current->next;
    }

    return 0;
}


/**
 * @brief Builds the ob file
 * @param ob_file a FILE* to write into 
 * @param code_image The code image
 * @param data_image The data image
 * @param ic The instruction counter
 * @param dc The data counter
*/

void build_ob(FILE *ob_file, machine_word **code_image, machine_word **data_image, int *ic, int *dc) {
    int i = 0;

    fprintf(ob_file, "%d %d\n", *ic - INITIAL_IC_VALUE, *dc);

    for (i = 0; i < *ic - INITIAL_IC_VALUE; i++)
    {
       machine_word *word = code_image[i];
       char *octal_word = convert_to_octal(word->data);
       fprintf(ob_file, "%04d %s\n", i + INITIAL_IC_VALUE, octal_word);
       safe_free(octal_word);
    }

    for (i = 0; i < *dc; i++)
    {
        machine_word *word = data_image[i];
        char *octal_word = convert_to_octal(word->data);
        fprintf(ob_file, "%04d %s\n", i + *ic, octal_word);
        safe_free(octal_word);
    }
}

/**
 * @brief Builds the entries file
 * @param ent_file a FILE* to write into 
 * @param labels The labels struct
*/
void build_entries(FILE *ent_file, Labels *labels) {
    LabelEntry *current = labels->head;

    while (current != NULL)
    {
        if (current->type == ENTRY_LABEL)
        {
            fprintf(ent_file, "%s %04d\n", current->key, current->value);
        }
        current = current->next;
    }

}

/**
 * @brief Builds the externals file
 * @param ext_file a FILE* to write into 
 * @param extern_usage The list of externals with where they were used
*/

void build_externals(FILE *ext_file, List *extern_usage) {
    Node *current = extern_usage->head;

    while (current != NULL) /* Iterate over the different externals that are being used */
    {
        /* Write into the file in a key-data format (key will be a 4 digit number)*/
        fprintf(ext_file, "%s %04d\n", current->key, *(int *) current->data);
        current = current->next;
    }
}

/**
    * @brief Creates the output files
    * @param file_name The name of the file
    * @param labels The labels struct
    * @param extern_usage The list of externals with where they were used
    * @param code_image The code image
    * @param data_image The data image
    * @param ic The instruction counter
    * @param dc The data counter
*/

void create_output_files(char *file_name, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int *ic, int *dc) {

    int i = 0;
    FILE *ob_file, *ent_file, *ext_file; /* The output files */

    int create_ent = 0, create_ext = 0; /* Flags to indicate if the files need to be created */
    char *ent_file_name = safe_malloc(strlen(file_name) + 2);
    char *ext_file_name = safe_malloc(strlen(file_name) + 2);
    char *ob_file_name = safe_malloc(strlen(file_name) + 1);


    ent_file_name[strlen(file_name) + 1] = '\0';
    ext_file_name[strlen(file_name) + 1] = '\0';
    ob_file_name[strlen(file_name)] = '\0';

    /* Copy the file name until the . */
    for (; i < strlen(file_name) && file_name[i] != '.'; i++)
    {
        ent_file_name[i] = file_name[i];
        ext_file_name[i] = file_name[i];
        ob_file_name[i] = file_name[i];
    }
    ent_file_name[i] = '\0';
    ext_file_name[i] = '\0';
    ob_file_name[i] = '\0';

    /* Add the file extensions (according to the file type) */
    strcat(ent_file_name, ".ent");
    strcat(ext_file_name, ".ext");
    strcat(ob_file_name, ".ob");




    /* Open the files */

    ob_file = open_file(ob_file_name, "w");

    build_ob(ob_file, code_image, data_image, ic, dc); /* Build the .ob file */

    /* Check what files need to be created and create the neccessary files */
    if (does_entry_exist(labels)) {
        create_ent = 1; 
        ent_file = open_file(ent_file_name, "w");
        build_entries(ent_file, labels);
    }
   
    if (does_extern_exist(extern_usage)) {
        create_ext = 1;
        ext_file = open_file(ext_file_name, "w");
        build_externals(ext_file, extern_usage);
    }

    /* Close the files & free the memory */
    if (create_ent) {
        fclose(ent_file);    
    }

    if (create_ext) {
        fclose(ext_file);
    }

    fclose(ob_file);

    safe_free(ent_file_name);
    safe_free(ext_file_name);
    safe_free(ob_file_name);
}