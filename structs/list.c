#include "list.h"

List* list_create() {
    List* list = safe_malloc(sizeof(List));
    list->head = NULL;
    return list;
}

void list_insert_string(List* list, char *key, char *data) {
    /* Append at the top */

    Node* new_node = safe_malloc(sizeof(Node));

    new_node->key = safe_malloc(strlen(key) + 1);
    new_node->data = safe_malloc(strlen(data) + 1);
    
    strcpy(new_node->key, key);
    strcpy(new_node->data, data);
    new_node->type = STRING_TYPE;
    
    new_node->next = list->head;
    list->head = new_node;

}

void list_insert_integer(List* list, char *key, int data) {
    /* Append at the top */

    Node* new_node = safe_malloc(sizeof(Node));

    new_node->key = safe_malloc(strlen(key) + 1);
    new_node->data = safe_malloc(sizeof(int));
    
    strcpy(new_node->key, key);


    *(int*) new_node->data = data;

    new_node->type = INTEGER_TYPE;
    
    new_node->next = list->head;
    list->head = new_node;


}

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

Node *list_get(List* list, char *key) {


    Node* current = list->head;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current; 
        }
        current = current->next;
    }

    return NULL;
}

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
