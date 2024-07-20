#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "logs.h"

void *safe_malloc(size_t size);
void *safe_realloc(void *ptr, size_t size);
void safe_free(void *ptr);

FILE *open_file(const char *filename, char *mode);
void close_file(FILE *file);



#endif
