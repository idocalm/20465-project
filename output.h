#ifndef OUTPUT_H
#define OUTPUT_H

#include "globals.h"
#include "./structs/list.h"
#include "./structs/labels.h"

void create_output_files(char *file_name, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int *ic, int *dc);

#endif
