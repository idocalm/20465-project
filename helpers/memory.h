#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "logs.h"

/* - Prototypes - */

void *safe_malloc(size_t size);
void *safe_realloc(void *ptr, size_t size);
void safe_free(void *ptr);

FILE *open_file(const char *filename, char *mode);
void close_file(FILE *file);

void free_code_image(machine_word **code_image, int ic);
void free_data_image(machine_word **data_image, int dc);
void free_operands(char **operands, int operands_count);



#endif
