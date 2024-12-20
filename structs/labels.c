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
    * Create a new blank labels list
    * @return A pointer to the new labels list
*/

Labels* labels_create() {
    Labels* labels = safe_malloc(sizeof(Labels));
    labels->head = NULL;
    return labels;
}

/**
    * Insert a new label into the labels list
    * @param labels - pointer to the struct 
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
    * Find a label by it's key and type in the labels list
    * @param labels - a pointer to the list 
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

/**
    * Search for a label without relation to the type
    * @param labels - a pointer to the structure
    * @param key - The label name 
    * @return A pointer to the label entry if found or NULL.
*/

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

/**
    * @brief This function adds the ic value to any DATA_LABEL labels 
    * @param labels The labels struct
    * @param ic The instruction counter

    * Note: this is done to create different "zones" in the system memory, one for the code and one for the data! 
    * Information stated at page 53 of the booklet

*/
void update_labels(Labels *labels, int ic)
{
    LabelEntry *current = labels->head;

    while (current != NULL)
    {
        /* Check that the label is indeed a DATA_LABEL before updating the value */
        if (current->type == DATA_LABEL)
        {
            current->value += ic;
        }
        current = current->next;
    }
}

/**
 * @brief Checks if an entry exists in the labels table 
 * @param labels The labels struct
 * @return 1 if an entry exists, 0 otherwise
*/

int does_entry_exist(Labels *labels) {
    LabelEntry *current = labels->head;

    while (current != NULL)
    {
        if (current->type == ENTRY_LABEL)
        {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

/**
    @brief Returns the length of the longest entry label 
    @param labels The labels struct
    @return The length of the longest label name (a whole number or zero)
*/

int get_longest_entry_label(Labels *labels) {
    LabelEntry *current = labels->head;
    int longest = 0; /* Length of the longest label name */

    while (current != NULL)
    {
        if (current->type == ENTRY_LABEL && strlen(current->key) > longest)
        {
            longest = strlen(current->key);
        }
        current = current->next;
    }

    return longest;
}

