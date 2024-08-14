#ifndef LIST_H
#define LIST_H

#include "../helpers/memory.h"
#include <string.h>

typedef enum {
    STRING_TYPE, 
    INTEGER_TYPE
} DataType;

typedef struct Node {
    char *key;
    void *data;
    DataType type;
    struct Node* next;
} Node;

typedef struct {
    Node *head;
} List;

/* - Prototypes - */

List* list_create();
void list_insert_string(List* list, char *key, char *data);
void list_insert_integer(List* list, char *key, int data);
void list_delete(List* list, char *key);
Node* list_get(List* list, char *key);
void list_free(List* list);
int is_list_empty(List *list);
int get_list_longest_key(List *list);

#endif
