#include "labels.h"

Labels* labels_create() {
    Labels* labels = safe_malloc(sizeof(Labels));
    labels->head = NULL;
    return labels;
}

void labels_insert(Labels* labels, char *key, int value, LabelType type) {
    LabelEntry* new_entry = safe_malloc(sizeof(LabelEntry));

    new_entry->key = safe_malloc(strlen(key) + 1);
    strcpy(new_entry->key, key);
    
    new_entry->value = value;
    new_entry->type = type;
    new_entry->next = labels->head;
    
    /* Append at the top */
    labels->head = new_entry;
}

void labels_delete(Labels* labels, char *key) {
    LabelEntry* current = labels->head;
    LabelEntry* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                labels->head = current->next;
            } else {
                prev->next = current->next;
            }
            safe_free(current->key);
            safe_free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

LabelEntry* labels_get(Labels* labels, char *key) {
    LabelEntry* current = labels->head;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void labels_free(Labels* labels) {
    LabelEntry* current = labels->head;
    LabelEntry* next = NULL;

    while (current != NULL) {
        next = current->next;
        safe_free(current->key);
        safe_free(current);
        current = next;
    }

    safe_free(labels);
}

void debug_labels(Labels* labels) {
    LabelEntry* current = labels->head;

    printf("--- DEBUG LABELS ---\n");


    printf("\033[0;31m");
    printf("--- TYPE: CODE_LABEL ---\n");
    printf("\033[0m");

    while (current != NULL) {
        if (current->type == CODE_LABEL) {
            printf("Key: %s\t\tValue: %d\n", current->key, current->value);
        }
        current = current->next;
    }

    printf("\033[0;32m");
    printf("--- TYPE: DATA_LABEL ---\n");
    printf("\033[0m");
    
    current = labels->head;
    while (current != NULL) {
        if (current->type == DATA_LABEL) {
            printf("Key: %s\t\tValue: %d\n", current->key, current->value);
        }
        current = current->next;
    }

    printf("\033[0;33m");
    printf("--- TYPE: EXTERN_LABEL ---\n");
    printf("\033[0m");

    current = labels->head;
    while (current != NULL) {
        if (current->type == EXTERN_LABEL) {
            printf("Key: %s\t\tValue: %d\n", current->key, current->value);
        }
        current = current->next;
    }

    printf("\033[0;34m");
    printf("--- TYPE: ENTRY_LABEL ---\n");
    printf("\033[0m");

    current = labels->head;
    while (current != NULL) {
        if (current->type == ENTRY_LABEL) {
            printf("Key: %s\t\tValue: %d\n", current->key, current->value);
        }
        current = current->next;
    }





    printf("--- END DEBUG LABELS ---\n");
}

