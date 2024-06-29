#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
#include "../syntax/helpers.h"

unsigned int hash(char *key)
{
    unsigned int hash = 0;
    int i;
    for (i = 0; key[i] != '\0'; i++)
    {
        hash = 31 * hash + key[i];
    }
    return hash % HASH_TABLE_SIZE;
}

HashTable *hashtable_init()
{
    int i = 0;
    HashTable *ht = (HashTable *)alloc(sizeof(HashTable));
    ht->size = 0;
    for (; i < HASH_TABLE_SIZE; i++)
    {
        ht->nodes[i] = NULL;
    }
    return ht;
}

HashTableNode *get_node(HashTable *ht, char *key)
{
    unsigned int index = hash(key);
    HashTableNode *node = ht->nodes[index];
    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
        {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void *put_node(HashTable *ht, HashTableNode *node)
{
    unsigned int index = hash(node->key);
    HashTableNode *old_node = get_node(ht, node->key);
    if (old_node != NULL)
    {
        old_node->value = node->value;
    }
    else
    {
        node->next = ht->nodes[index];
        ht->nodes[index] = node;
        ht->size++;
    }
}

void hashtable_putint(HashTable *ht, char *key, int value)
{
    HashTableNode *node = (HashTableNode *)alloc(sizeof(HashTableNode));
    node->key = alloc(strlen(key));
    strcpy(node->key, key);
    node->value = alloc(sizeof(value));
    *(int *)node->value = value;
    put_node(ht, node);
}

int *hashtable_getint(HashTable *ht, char *key)
{
    HashTableNode *node = get_node(ht, key);
    if (node == NULL)
    {
        return NULL;
    }
    return (int *)node->value;
}

void hashtable_putstr(HashTable *ht, char *key, char *value)
{
    HashTableNode *node = (HashTableNode *)alloc(sizeof(HashTableNode));
    node->key = alloc(strlen(key));
    strcpy(node->key, key);
    node->value = (char *)alloc(strlen(value));
    strcpy(node->value, value);
    put_node(ht, node);
}

char *hashtable_getstr(HashTable *ht, char *key)
{
    HashTableNode *node = get_node(ht, key);
    printf("key: %s\n", key);
    if (node == NULL)
    {
        return NULL;
    }
    return node->value;
}

void hashtable_free(HashTable *ht)
{
    printf("ht is %p\n", ht);
    if (ht == NULL)
        return;

    printf("Freeing hashtable\n");
    printf("Size: %d\n", ht->size);

    int i;
    HashTableNode *node, *next;
    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        node = ht->nodes[i];
        printf("Freeing node %d\n", i);

        while (node != NULL)
        {
            printf("Node key: %s, with value: %s \n", node->key, node->value);
            next = node->next;
            free(node->key);
            free(node->value);
            free(node);
            node = next;
        }
    }
    free(ht);
}
