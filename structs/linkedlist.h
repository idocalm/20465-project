#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct {
    void *data;
    struct Node *next;
} Node;

void appendAtStart(Node **head, void *data);
void appendAtEnd(Node **head, void *data);

void freeList(Node *head);


#endif