#ifndef COMMANDS_H
#define COMMANDS_H


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../structs/list.h"

#include "../globals.h"
#include "../helpers/logs.h"
#include "../helpers/symbols.h"
#include "../helpers/strings.h"

char *is_label(char *line);
void get_operands(char *line, char **operands, int *operandsCount);
OperationGroup get_operation_group(Operation op);
void handle_directive_line(char *line, int lineNum, int *p_ic, List *p_labels, List *p_macros, int isFirstWord); 
void handle_instruction_line(char *line);
int is_valid_label(char *label, List *p_macros);

AddressMode find_addressing_mode(char *operand, List *p_macros);

#endif