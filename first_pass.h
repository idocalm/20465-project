#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>


#include "structs/list.h"
#include "globals.h"
#include "syntax/command_line.h"
#include "syntax/data_line.h"



PassError first_pass(char *p_fileName, int *ic, int *dc, Labels *labels, List *macros, machine_word **code_image, machine_word **data_image);


#endif 