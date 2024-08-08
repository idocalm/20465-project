#include "list.h"

/*
    - The list structure is a typical linked list. Nothing fancy here, 
    except we have a type field which can be either STRING_TYPE or INTEGER_TYPE.
    So we can store either a string or an integer as data.

    That comes in handy because we can use the same structure for storing macros (which is a string)
    and for storing the extern usage (which is an integer).

*/


/**
    * Create a new blank list structure
    * @return A pointer to the new list structure
*/

List* list_create() {
    List* list = safe_malloc(sizeof(List));
    list->head = NULL;
    return list;
}

/**
    * Insert a string
    * @param list - pointer to a list
    * @param key - The key of the new node
    * @param data - The data of the new node
*/

void list_insert_string(List* list, char *key, char *data) {
    /* Append at the top */

    Node* new_node = safe_malloc(sizeof(Node));

    new_node->key = safe_malloc(strlen(key) + 1);
    new_node->data = safe_malloc(strlen(data) + 1);

    /* Copy the key & data */
    strcpy(new_node->key, key);
    strcpy(new_node->data, data);
    new_node->type = STRING_TYPE;
    
    new_node->next = list->head;
    list->head = new_node;
}

/**
    * Insert an integer 
    * @param list - pointer to a list
    * @param key - The key of the new node
    * @param data - The data of the new node
*/
void list_insert_integer(List* list, char *key, int data) {
    /* Append at the top */

    Node* new_node = safe_malloc(sizeof(Node));

    new_node->key = safe_malloc(strlen(key) + 1);
    new_node->data = safe_malloc(sizeof(int));
    
    /* Here we only need to copy */
    strcpy(new_node->key, key);


    *(int*) new_node->data = data;

    new_node->type = INTEGER_TYPE;
    
    new_node->next = list->head;
    list->head = new_node;


}

/**
    * Delete a node from a list
    * @param list - pointer to a list
    * @param key - The key to delete
 */
void list_delete(List* list, char *key) {
    Node* current = list->head;
    Node* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                list->head = current->next;
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

/** 
    * Find a node inside a list
    * @param list - pointer to a list
    * @param key - The key to search
*/

Node *list_get(List* list, char *key) {
    Node* current = list->head;

    while (current != NULL) {
        /* Compare the keys */
        if (strcmp(current->key, key) == 0) {
            return current; 
        }
        current = current->next;
    }

    return NULL;
}

/**
    * Frees everything that the list contains and itself.
    * @param list - A pointer to a list 
*/

void list_free(List* list) {
    Node* current = list->head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        safe_free(current->key);
        safe_free(current->data);
        safe_free(current);
        current = next;
    }

    safe_free(list);
}

/* TODO: Delete this part */
void list_print(List* list) {
    Node* current = list->head;
    printf("Printing list\n");

    while (current != NULL) {
        printf("%s\n", current->key);
        printf("%d\n", *(int*) current->data);
        current = current->next;
    }

    printf("End of list\n");
}
