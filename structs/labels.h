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
void labels_insert(Labels* labels, char *key, int value, LabelType type);
LabelEntry* labels_get(Labels* labels, char *key);
void labels_delete(Labels* labels, char *key);
void labels_free(Labels* labels);
void debug_labels(Labels* labels);

#endif