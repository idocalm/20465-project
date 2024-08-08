#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>


#include "structs/list.h"
#include "structs/labels.h"
#include "definitions.h"

#include "helpers/memory.h"
#include "helpers/strings.h"
#include "helpers/symbols.h"

#include "first_pass.h"

PassError second_pass(char *file_name, Labels *labels, List *extern_usage, machine_word **code_image, machine_word **data_image, int first_pass_error);

#endif