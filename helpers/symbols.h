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

/* - Prototypes - */

Operation get_operation(char *str);
Directive get_directive(char *str);
Register get_register(char *str);
int is_reserved_word(char *str);
int is_label_error(char *label, int line_num, char *dest, int report_error);
int is_label(char *label);
int valid_command_with_operands(Operation op, AddressMode dest, AddressMode source);


OperationGroup get_operation_group(Operation op);
AddressMode address_mode(char *operand);

#endif