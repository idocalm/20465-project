#include "output.h"

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

    int contains_entry = 0; /* Do we need to create an .entry file? */
    int contains_extern = 0; /* Do we need to create an .extern file? */
    int i = 0;
    LabelEntry *current = labels->head; /* The current label entry (we would soon iterate over this). */
    FILE *ob_file, *ent_file, *ext_file; /* The output files */

    char *ent_file_name = safe_malloc(strlen(file_name) + 1);
    char *ext_file_name = safe_malloc(strlen(file_name) + 1);
    char *ob_file_name = safe_malloc(strlen(file_name) + 2);

    /* Copy the file name until the . */
    for (; i < strlen(file_name) && file_name[i] != '.'; i++)
    {
        ent_file_name[i] = file_name[i];
        ext_file_name[i] = file_name[i];
        ob_file_name[i] = file_name[i];
    }

    /* Add the file extensions (according to the file type) */
    ent_file_name[i] = '.';
    ext_file_name[i] = '.';
    ob_file_name[i] = '.';

    ent_file_name[i + 1] = 'e';
    ent_file_name[i + 2] = 'n';
    ent_file_name[i + 3] = 't';
    ent_file_name[i + 4] = '\0';


    ext_file_name[i + 1] = 'e';
    ext_file_name[i + 2] = 'x';
    ext_file_name[i + 3] = 't';
    ext_file_name[i + 4] = '\0';    

    ob_file_name[i + 1] = 'o';
    ob_file_name[i + 2] = 'b';
    ob_file_name[i + 3] = '\0';

    /* Open the files */

    ob_file = open_file(ob_file_name, "w");
    ent_file = open_file(ent_file_name, "w");
    ext_file = open_file(ext_file_name, "w");


    /* Check what files need to be created */

    while (current != NULL)
    {
        if (current->type == ENTRY_LABEL)
        {
            contains_entry = 1;
        }
        else if (current->type == EXTERN_LABEL)
        {
            contains_extern = 1;
        }
        current = current->next;
    }

    build_ob(ob_file, code_image, data_image, ic, dc);

    if (contains_entry)
    {
        build_entries(ent_file, labels);
    }

    if (contains_extern)
    {
        build_externals(ext_file, extern_usage);
    }

    /* Close the files & free the memory */

    fclose(ob_file);
    fclose(ent_file);
    fclose(ext_file);

    safe_free(ent_file_name);
    safe_free(ext_file_name);
    safe_free(ob_file_name);
}