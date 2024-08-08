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

void build_ob(FILE *ob_file, machine_word **code_image, machine_word **data_image, int *ic, int *dc) {
    int i = 0;

    fprintf(ob_file, "%d %d\n", *ic - INITIAL_IC_VALUE, *dc);

    for (i = 0; i < *ic - INITIAL_IC_VALUE; i++)
    {
       machine_word *word = code_image[i];
       char *octal_word = convert_to_octal(word->data);
       char *formatted_address = format_address(i + INITIAL_IC_VALUE);
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
}

void build_entries(FILE *ent_file, Labels *labels) {
    LabelEntry *current = labels->head;

    while (current != NULL)
    {
        if (current->type == ENTRY_LABEL)
        {
            fprintf(ent_file, "%s %d\n", current->key, current->value);
        }
        current = current->next;
    }

}

void build_externals(FILE *ext_file, List *extern_usage) {
    int i = 0;
    Node *current = extern_usage->head;

    while (current != NULL)
    {
        char *formatted_address = format_address(*(int *) current->data);

        fprintf(ext_file, "%s %s\n", current->key, formatted_address);
        current = current->next;
        safe_free(formatted_address);
    }
}

void create_output_files(char *file_name, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int *ic, int *dc) {

    int contains_entry = 0;
    int contains_extern = 0;
    int i = 0;
    LabelEntry *head = labels->head;
    LabelEntry *current = head;

    char *ent_file_name = safe_malloc(strlen(file_name) + 1);
    char *ext_file_name = safe_malloc(strlen(file_name) + 1);
    char *ob_file_name = safe_malloc(strlen(file_name) + 2);

    for (; i < strlen(file_name) && file_name[i] != '.'; i++)
    {
        ent_file_name[i] = file_name[i];
        ext_file_name[i] = file_name[i];
        ob_file_name[i] = file_name[i];
    }

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


    debug_labels(labels);

    FILE *ob_file = open_file(ob_file_name, "w");
    FILE *ent_file = open_file(ent_file_name, "w");
    FILE *ext_file = open_file(ext_file_name, "w");


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
}