#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "../globals.h"
#include "../structs/list.h"
#include "../structs/labels.h"
#include "./strings.h"

#define ADDRESSING_MODES 4

typedef struct {
    char *name;
    Operation operation;
} operation_table;

typedef struct {
    char *name;
    Instruction instruction;
} instruction_table;


typedef struct {
    Operation operation;
    int source_modes[ADDRESSING_MODES];
    int dest_modes[ADDRESSING_MODES];
} valid_command_modes;

/* - Prototypes - */

Operation get_operation(char *str);
Instruction get_instruction(char *str);
Register get_register(char *str);
int is_reserved_word(char *str);
int is_label_error(char *line, int line_num, char *dest, int report_error);
int is_label(char *label);
int valid_command_with_operands(Operation op, AddressMode dest, AddressMode source);


OperationGroup get_operation_group(Operation op);
AddressMode address_mode(char *operand);

#endif