#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "../definitions.h"

typedef struct {
    char *name;
    Operation operation;
} operation_search;

typedef struct {
    char *name;
    Directive directive;
} directive_search;

Operation get_operation(char *str);
Directive get_directive(char *str);
Register get_register(char *str);

void symbols_init();
void symbols_free();

#endif