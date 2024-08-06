#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>


#include "structs/list.h"
#include "structs/labels.h"
#include "globals.h"

#include "helpers/strings.h"
#include "first_pass.h"

PassError second_pass(char *file_name, List* macros, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image);

#endif