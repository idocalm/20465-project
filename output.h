#ifndef OUTPUT_H
#define OUTPUT_H

#include "definitions.h"
#include "./structs/list.h"
#include "./structs/labels.h"
#include "./helpers/strings.h"
#define OCTAL_SIZE 5

/* - Prototypes - */

void build_ob(FILE *ob_file, machine_word **code_image, machine_word **data_image, int *ic, int *dc);
void build_entries(FILE *ent_file, Labels *labels);
void build_externals(FILE *ext_file, List *extern_usage);
void create_output_files(char *file_name, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int *ic, int *dc);

#endif
