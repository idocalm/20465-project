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

    /* Note that using & 0x7FFF is the hex of a binary number with 15 1's from the right */
    /* We apply it with & bit operation so the result could be used by %o to print in octal*/

    /* Align the counters so they will be printed in the correct format */
    int digit_amount = 0;
    int temp = *ic - INITIAL_IC_VALUE;
    while (temp != 0) {
        temp /= 10;
        digit_amount++;
    }

    for (i = 0; i < 4 - digit_amount; i++)
        fprintf(ob_file, " ");

    fprintf(ob_file, "%d %d", *ic - INITIAL_IC_VALUE, *dc);

    for (i = 0; i < *ic - INITIAL_IC_VALUE; i++)
    {
       machine_word *word = code_image[i];
       fprintf(ob_file, "\n%04d %05o", i + INITIAL_IC_VALUE, word->data & 0x7FFF);
    }

    for (i = 0; i < *dc; i++)
    {
        machine_word *word = data_image[i];
        fprintf(ob_file, "\n%04d %05o", i + *ic, word->data & 0x7FFF);
    }
}

/**
 * @brief Builds the entries file
 * @param ent_file a FILE* to write into 
 * @param labels The labels struct
*/
void build_entries(FILE *ent_file, Labels *labels) {
    LabelEntry *current = labels->head;
    int longest = get_longest_entry_label(labels); /* Length of the longest label name */
    int i; 
    int first = 1; /* First in the list */

    current = labels->head;
    while (current != NULL)
    {
        if (current->type == ENTRY_LABEL)
        {
            if (first) {
                fprintf(ent_file, "%s", current->key);
                first = 0;
            } else {
                fprintf(ent_file, "\n%s", current->key);
            }
            /* The instructions provided on formatting the .ent file are simple not clear. 
                We chose to format the file so all the address will appear in the same location on the line  */
            for (i = 0; i < longest - strlen(current->key); i++)
                fprintf(ent_file, " ");
            fprintf(ent_file, " %04d", current->value);
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
    int i = 0;
    int longest = get_list_longest_key(extern_usage); /* Length of the longest label name */
    int first = 1; /* First in the list */

    while (current != NULL) /* Iterate over the different externals that are being used */
    {
        if (first) {
            fprintf(ext_file, "%s", current->key);
            first = 0;
        } else {
            fprintf(ext_file, "\n%s", current->key);
        }
        for (i = 0; i < longest - strlen(current->key); i++)
            fprintf(ext_file, " ");
        /* Write into the file in a key-data format (key will be a 4 digit number)*/
            fprintf(ext_file, " %04d", *(int*) current->data);
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

    FILE *ob_file, *ent_file, *ext_file; /* The output files */

    int create_ent = 0, create_ext = 0; /* Flags to indicate if the files need to be created */
    size_t len = strcspn(file_name, "."); /* The length of the file name without the .am */
    char *ent_file_name = safe_malloc(len + 5); /* +5 --> .ent\0 */
    char *ext_file_name = safe_malloc(len + 5); /* +5 --> .ext\0 */
    char *ob_file_name = safe_malloc(len + 4); /* +4 --> .ob\0 */

    /* Copy the file name */
    strncpy(ent_file_name, file_name, len);
    strncpy(ext_file_name, file_name, len);
    strncpy(ob_file_name, file_name, len);

    /* Add .ob / .ent / .ext */
    sprintf(ent_file_name + len, ".ent");
    sprintf(ext_file_name + len, ".ext");
    sprintf(ob_file_name + len, ".ob");

    /* Open the files */

    ob_file = open_file(ob_file_name, "w");

    build_ob(ob_file, code_image, data_image, ic, dc); /* Build the .ob file */

    /* Check what files need to be created and create the neccessary files */
    if (does_entry_exist(labels)) {
        create_ent = 1; 
        ent_file = open_file(ent_file_name, "w");
        build_entries(ent_file, labels);
    }
   
    if (!is_list_empty(extern_usage)) {
        create_ext = 1;
        ext_file = open_file(ext_file_name, "w");
        build_externals(ext_file, extern_usage);
    }
    /* Close the files & free the memory */
    if (create_ent)
        fclose(ent_file);    

    if (create_ext)
        fclose(ext_file);

    fclose(ob_file);
    
    safe_free(ent_file_name);
    safe_free(ext_file_name);
    safe_free(ob_file_name);
}