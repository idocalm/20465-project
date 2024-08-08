#ifndef CODE_LINE_H
#define CODE_LINE_H

#include <stddef.h>
#include <string.h>
#include "../structs/list.h"
#include "../helpers/strings.h"
#include "../structs/labels.h"
#include "../helpers/symbols.h"
#include "../definitions.h"

/* - Prototypes - */

machine_word *build_single_word(char *source, char *dest, int *ic);
machine_word *build_additional_word(AddressMode mode, int is_dest, char *operand, int *ic);
machine_word *build_first_word(Operation op, AddressMode source, AddressMode dest, int *ic);
void add_to_code_image(machine_word **code_image, machine_word *word, int *ic, int *found_error);
void extract_address_modes(char **operands, int operands_count, AddressMode *dest, AddressMode *source, int line_num, int *found_error);
int handle_code_line(char *line, int line_num, int *ic, Labels *labels, machine_word **code_image);

#endif 