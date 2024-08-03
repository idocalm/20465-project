#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <stddef.h>
#include <string.h>
#include "../structs/list.h"
#include "../structs/labels.h"
#include "../helpers/symbols.h"
#include "../globals.h"

void get_operands(char *line, char **operands, int *operandsCount);
machine_word *build_single_word(char *source, char *dest, int *ic, int line_num);
machine_word *build_additional_word(AddressMode mode, int is_dest, char *operand, int *ic, int line_num);
machine_word *build_first_word(Operation op, AddressMode source, AddressMode dest, List *macros, int *ic, int line_num);
int handle_directive_line(char *line, int line_num, int *ic, Labels *labels, List *macros, machine_word **code_image);


#endif 