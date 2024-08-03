#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "../globals.h"
#include "../structs/list.h"
#include "./strings.h"

#define ADDRESSING_MODES 4

typedef struct {
    char *name;
    Operation operation;
} operation_search;

typedef struct {
    char *name;
    Directive directive;
} directive_search;


typedef struct {
    Operation operation;
    int source_modes[ADDRESSING_MODES];
    int dest_modes[ADDRESSING_MODES];
} valid_command_modes;

Operation get_operation(char *str);
Directive get_directive(char *str);
Register get_register(char *str);


int is_reserved_word(char *str);
int valid_command_with_operands(Operation op, AddressMode dest, AddressMode source);
char *is_label_def(char *label, List *p_macros);
int is_label(char *label);

OperationGroup get_operation_group(Operation op);
AddressMode find_addressing_mode(char *operand);

#endif