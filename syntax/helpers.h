#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

void *alloc(size_t size);
FILE *open_file(const char *fileName, const char *mode);
void skip_spaces(char **p);

#endif