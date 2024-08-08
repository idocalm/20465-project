#include "output.h"

#define OCTAL_SIZE 5
#define ADDRESS_SIZE 4

char *convert_to_octal(int data) {
    char *octal = safe_malloc(OCTAL_SIZE + 1);
    int i = 0;
    int mask = 0x7;

    for (i = 0; i < OCTAL_SIZE; i++)
    {
        octal[i] = '0';
    }
    octal[OCTAL_SIZE] = '\0';

    for (i = 0; i < OCTAL_SIZE; i++)
    {
        octal[OCTAL_SIZE - i - 1] = (data & mask) + '0';
        data >>= 3;
    }


    return octal;
}

char *format_address(int address) {
    int temp = address;
    int i;
    char *res = safe_malloc(ADDRESS_SIZE + 1);
    memset(res, '0', ADDRESS_SIZE);

    res[ADDRESS_SIZE] = '\0';

    for (i = ADDRESS_SIZE - 1; i >= 0; i--)
    {
        res[i] = (temp % 10) + '0';
        temp /= 10;
    }

    return res;
}

void build_ob(char *file_name, machine_word **code_image, machine_word **data_image, int *ic, int *dc) {
    FILE *ob_file;
    int i = 0;

    char *ob_file_name = safe_malloc(strlen(file_name) + 2);
    for (i = 0; i < strlen(file_name) && file_name[i] != '.'; i++)
    {
        ob_file_name[i] = file_name[i];
    }
    ob_file_name[i] = '.';
    ob_file_name[i + 1] = 'o';
    ob_file_name[i + 2] = 'b';


    ob_file = open_file(file_name, "w");

    fprintf(ob_file, "%d %d\n", *ic - INITIAL_IC_VALUE, *dc);

    for (i = 0; i < *ic - INITIAL_IC_VALUE; i++)
    {
       machine_word *word = code_image[i];
       char *octal_word = convert_to_octal(word->data);
       char *formatted_address = format_address(i + INITIAL_IC_VALUE);
       printf("%s %s\n", formatted_address, octal_word);
       fprintf(ob_file, "%s %s\n", formatted_address, octal_word);
       safe_free(octal_word);
       safe_free(formatted_address);
    }


    for (i = 0; i < *dc; i++)
    {
        machine_word *word = data_image[i];
        char *octal_word = convert_to_octal(word->data);
        char *formatted_address = format_address(i + *ic);
        fprintf(ob_file, "%s %s\n", formatted_address, octal_word);
        safe_free(octal_word);
        safe_free(formatted_address);
        fflush(ob_file);
    }

    printf("Wrote to file %s\n", ob_file_name);

    close_file(ob_file);
    safe_free(ob_file_name);

}

void build_entries(char *file_name, Labels *labels) {

    FILE *ent_file; 
    char *ent_name = safe_malloc(strlen(file_name) + 2);
    int i = 0;
    LabelEntry *current = labels->head;

    for (i = 0; i < strlen(file_name) && file_name[i] != '.'; i++)
        ent_name[i] = file_name[i];

    ent_name[i] = '.';
    ent_name[i + 1] = 'e';
    ent_name[i + 2] = 'n';
    ent_name[i + 3] = 't';
    ent_name[i + 4] = '\0';


    ent_file = open_file(ent_name, "w");

    while (current != NULL)
    {
        if (current->type == ENTRY_LABEL)
        {
            fprintf(ent_file, "%s %d\n", current->key, current->value);
        }
        current = current->next;
    }

    close_file(ent_file);
    safe_free(ent_name);
}

void build_externals(char *file_name, List *extern_usage) {
    FILE *ext_file;
    char *ext_name = safe_malloc(strlen(file_name) + 2);
    int i = 0;
    Node *current = extern_usage->head;


    for (i = 0; i < strlen(file_name) && file_name[i] != '.'; i++)
        ext_name[i] = file_name[i];

    ext_name[i] = '.';
    ext_name[i + 1] = 'e';
    ext_name[i + 2] = 'x';
    ext_name[i + 3] = 't';
    ext_name[i + 4] = '\0';

    ext_file = open_file(ext_name, "w");

    while (current != NULL)
    {
        char *formatted_address = format_address(*(int *) current->data);

        fprintf(ext_file, "%s %s\n", current->key, formatted_address);
        current = current->next;
        safe_free(formatted_address);
    }

    close_file(ext_file);
    safe_free(ext_name);

}

void create_output_files(char *file_name, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int *ic, int *dc) {

    int contains_entry = 0;
    int contains_extern = 0;
    LabelEntry *head = labels->head;
    LabelEntry *current = head;

    debug_labels(labels);

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

    build_ob(file_name, code_image, data_image, ic, dc);

    /*

    if (contains_entry)
    {
        build_entries(file_name, labels);
    }

    if (contains_extern)
    {
        build_externals(file_name, extern_usage);
    }*/
}