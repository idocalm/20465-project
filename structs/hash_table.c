#include "hash_table.h"
#include "syntax/helpers.h"

unsigned int hash(char *key) {
    unsigned int hash = 0;
    int i;
    for (i = 0; key[i] != '\0'; i++) {
        hash = 31 * hash + key[i];
    }
    return hash % HashTable_SIZE;
}

HashTable *hashtable_init() {
    HashTable *ht = (HashTable *) alloc(sizeof(HashTable));
    ht->size = 0;
    int i = 0;
    for (; i < HASH_TABLE_SIZE; i++)
    {
        ht->nodes[i] = NULL;
    }
    return hash_table;
}

HashTableNode *get_node(HashTable *ht, char *key) {
    unsigned int index = hash(key);
    HashTableNode *node = ht->nodes[index];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void *put_node(HashTable *ht, HashTableNode *node) {
    unsigned int index = hash(node->key);
    HashTableNode *old_node = get_node(ht, node->key);
    if (old_node != NULL) {
        old_node->value = node->value;
        return;
    }
    node->next = ht->nodes[index];
    ht->nodes[index] = node;
    ht->size++;
}

void hashtable_putint(HashTable *ht, char *key, int value) {
    unsigned int index = hash(key);
    HashTableNode *node = (HashTableNode *) alloc(sizeof(HashTableNode));
    node->key = key;
    node->value = value;
    put_node(ht, node);
}

int hashtable_getint(HashTable *ht, char *key) {
    unsigned int index = hash(key);
    HashTableNode *node = get_node(ht, key);
    if (node == NULL) {
        return -1;
    }
    return node->value;
}

void hashtable_putstr(HashTable *ht, char *key, char *value) {
    unsigned int index = hash(key);
    HashTableNode *node = (HashTableNode *) alloc(sizeof(HashTableNode));
    node->key = key;
    node->value = value;
    put_node(ht, node);
}

char *hashtable_getstr(HashTable *ht, char *key) {
    unsigned int index = hash(key);
    HashTableNode *node = get_node(ht, key);
    if (node == NULL) {
        return NULL;
    }
    return node->value;
}

void hashtable_free(HashTable *ht) {
    int i;
    for (i=0; i < HASH_TABLE_SIZE; i++)
    {
        HashTableNode *node = ht->nodes[i];
        while (node != NULL) {
            HashTableNode *next = node->next;
            free(node);
            node = next;
        }
    }
    free(ht);
}