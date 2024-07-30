#ifndef SYMBOLS_H
#define SYMBOLS_H


#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "../globals.h"

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
    int source_modes[4];
    int dest_modes[4];
} valid_command_modes;

Operation get_operation(char *str);
Directive get_directive(char *str);
Register get_register(char *str);


int is_reserved_word(char *str);
int valid_command_with_operands(Operation op, AddressMode dest, AddressMode source);

void symbols_init();
void symbols_free();

#endif