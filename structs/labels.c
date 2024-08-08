#include "labels.h"

/*
    - The labels structure can be seen as a linked list, just instead of storing an int / string 
    it stores an entry with:

        a key - the label's name 
        a value - the label's address
        a type - the label's type (CODE_LABEL, DATA_LABEL, EXTERN_LABEL, ENTRY_LABEL)

    The type parameter is used so we can tell how was a label defined (by .extern or .entry or inside the code itself).
*/


/**
    * Create a new blank labels structure
    * @return A pointer to the new labels structure
*/

Labels* labels_create() {
    Labels* labels = safe_malloc(sizeof(Labels));
    labels->head = NULL;
    return labels;
}

/**
    * Insert a new label into the labels structure
    * @param labels - pointer to the structure
    * @param key - The label's "name"
    * @param value - The label's address (an integer)
    * @param type - The label's type
*/
void labels_insert(Labels* labels, char *key, int value, LabelType type) {
    LabelEntry* new_entry = safe_malloc(sizeof(LabelEntry));

    new_entry->key = safe_malloc(strlen(key) + 1);
    strcpy(new_entry->key, key); /* Copy the key */
    new_entry->key[strlen(key)] = '\0';
    
    new_entry->value = value;
    new_entry->type = type;
    new_entry->next = labels->head;
    
    /* Append at the top */
    labels->head = new_entry;
}

/**
    * Find a label by it's key and type in the labels structure
    * @param labels - a pointer
    * @param key - The label name
    * @param type - The label type
    * @return A pointer to the label entry if found or NULL. 
*/

LabelEntry* labels_get(Labels* labels, char *key, LabelType type) {
    LabelEntry* current = labels->head;

    /* Compare labels in the list until you find the matching one and return it */

    while (current != NULL) {
        if (strcmp(current->key, key) == 0 && current->type == type) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

LabelEntry* labels_get_any(Labels* labels, char *key) {
    LabelEntry* current = labels->head;

    /* Compare labels in the list until you find the matching one and return it */

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}


/**
    * Free the struct itself and everything in it
    * @param labels - A pointer to the structure 
 */
void labels_free(Labels* labels) {
    LabelEntry* current = labels->head;
    LabelEntry* next = NULL;

    while (current != NULL) {
        next = current->next;
        /* Free the key and the entry itself */
        safe_free(current->key);
        safe_free(current);
        current = next;
    }

    /* Free the whole structure */
    safe_free(labels);
}


/* TODO: Delete this */

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

