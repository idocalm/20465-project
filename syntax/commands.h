#ifndef COMMANDS_H
#define COMMANDS_H


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "../structs/hash_table.h"
#include "../globals.h"
#include "../helpers/logs.h"
#include "../helpers/symbols.h"

char *is_label(char *line);
void **get_operands(char *line, char **operands, int *operandsCount);
OperationGroup get_operation_group(Operation op);
void handle_directive_line(char *line, int lineNum, int *p_ic, ht_t *p_labels, ht_t *p_macros);
void handle_instruction_line(char *line);

#endif