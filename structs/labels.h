#ifndef LABELS_H
#define LABELS_H

#include <stddef.h>
#include <string.h>

#include "../helpers/memory.h"  

typedef enum {
    CODE_LABEL,
    DATA_LABEL,
    EXTERN_LABEL,
    ENTRY_LABEL
} LabelType;

typedef struct LabelEntry {
    LabelType type;
    int value; 
    char *key; 
    struct LabelEntry *next;
} LabelEntry;

typedef struct {
    LabelEntry *head;
} Labels; 

Labels* labels_create();
void labels_insert(Labels* symbols, char *key, int value, LabelType type);
LabelEntry* labels_get(Labels* symbols, char *key);
void labels_delete(Labels* symbols, char *key);
void labels_free(Labels* symbols);
void debug_labels(Labels* symbols);

#endif