#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define HASH_TABLE_SIZE 255 /* TODO: Make the size dynamic */

typedef struct {
    char *key;
    void *value;
    HashTableNode *next;
} HashTableNode;

typedef struct {
    HashTableNode *nodes[HASH_TABLE_SIZE];
    int size;
} HashTable;

unsigned int hash(char *key);
HashTable *hashtable_init();
HashTableNode *get_node(HashTable *ht, char *key);
void *put_node(HashTable *ht, HashTableNode *node);
void hashtable_putint(HashTable *ht, char *key, int value);
int hashtable_getint(HashTable *ht, char *key);
void hashtable_putstr(HashTable *ht, char *key, char *value);
char *hashtable_getstr(HashTable *ht, char *key);
void hashtable_free(HashTable *ht);

#endif 