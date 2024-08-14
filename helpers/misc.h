#ifndef MISC_H
#define MISC_H

#include <stddef.h>
#include <string.h>

#include "../definitions.h"
#include "./strings.h"
#include "./symbols.h"

int validate_operand_list(char *line, int line_num, int report_error);
int get_operands(char *line, char operands[MAX_OPERANDS][MAX_LINE_SIZE + 1], int *operands_count, int line_num);
int validate_int_size(int value, int signed_min, int unsigned_min, int unsigned_max);

void validate_immediate(AddressMode mode, char *operand, int line_num, int *found_error); 
void validate_string_instruction(char *line, int line_num, int *found_error);
void add_to_data_image(machine_word **data_image, int *dc, int data);
void add_to_code_image(machine_word **code_image, machine_word *word, int *ic, int *found_error);
void extract_address_modes(char operands[MAX_OPERANDS][MAX_LINE_SIZE + 1], int operands_count, AddressMode *dest, AddressMode *source, int line_num, int *found_error);

#endif 