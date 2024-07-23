#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>


#include "structs/list.h"
#include "globals.h"
#include "syntax/line_parser.h"


typedef enum {
    NO_FPASS_ERROR
} FPassErrors;

FPassErrors first_pass(char *p_fileName, int *p_ic, int *p_dc, List *p_labels, List *p_macros);


#endif 