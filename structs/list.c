#include "list.h"

/*
    - The list struct is a typical linked list. Nothing fancy here, 
    except we have a type field which can be either STRING_TYPE or INTEGER_TYPE.
    So we can store either a string or an integer as data.

    That comes in handy because we can use the same struct for storing macros (which is a string)
    and for storing the extern usage (which is an integer, the usage address).
*/

/**
    * @brief Create a new blank list 
    * @return A pointer to the new list
*/

List* list_create() {
    List* list = safe_malloc(sizeof(List));
    list->head = NULL;
    return list;
}

/**
    * @brief Insert a string at the begining of the list
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
    * @brief Insert an integer 
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
    * @brief Delete a node from a list
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
    * @brief Find a node inside a list
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
    * @brief Frees everything that the list contains and itself.
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

/**
    * @brief Checks if the list is empty
    * @param list the list 
    * @return 1 if an extern exists, 0 otherwise
*/

int is_list_empty(List *list) {
    Node *current = list->head;

    if (current == NULL || current->data == NULL) {
        return 1;
    }

    return 0;
}


/**
    @brief Returns the len of the longest key entry in the list
    @param list - The list 
    @return The length of the longest key (int)
*/

int get_list_longest_key(List *list) {
    Node *current = list->head;
    int longest = 0;

    while (current != NULL) {
        if (strlen(current->key) > longest) {
            longest = strlen(current->key);
        }
        current = current->next;
    }

    return longest;
}


