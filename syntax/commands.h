#ifndef COMMANDS_H
#define COMMANDS_H


#include "../structs/hash_table.h"
#include "../definitions.h"

char *is_label(char *line);
void **get_operands(char *line, char **operands, int *operandsCount);
OperationGroup get_operation_group(Operation op);
void handle_directive_line(char *line, int lineNum, ht_t *p_macros);
void handle_instruction_line(char *line);

#endif