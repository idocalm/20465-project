#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define HASH_TABLE_SIZE 100

typedef struct entry_t
{
    char *key;
    char *value;
    struct entry_t *next;
} entry_t;

typedef struct
{
    entry_t **entries;
} ht_t;

ht_t *ht_create(void);
char *ht_get(ht_t *hashtable, const char *key);
void ht_set(ht_t *hashtable, const char *key, const char *value);
void ht_del(ht_t *hashtable, const char *key);
void ht_free(ht_t *hashtable);


#endif