#ifndef LABELS_H
#define LABELS_H

#include <stddef.h>
#include <string.h>

#include "../helpers/memory.h"  

typedef enum {
    CODE_LABEL, /* A label defined at the start of a code line */
    DATA_LABEL,  /* .data / .string */
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

/* - Prototypes */

Labels* labels_create();
void labels_insert(Labels* labels, char *key, int value, LabelType type);
LabelEntry* labels_get(Labels* labels, char *key, LabelType type);
LabelEntry* labels_get_any(Labels* labels, char *key);
void labels_free(Labels* labels);
void update_labels(Labels *labels, int ic);
int does_entry_exist(Labels *labels);
int get_longest_entry_label(Labels *labels);

#endif