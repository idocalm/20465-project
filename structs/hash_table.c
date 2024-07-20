#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

unsigned int hash(const char *key)
{
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; ++i)
    {
        value = value * 37 + key[i];
    }

    value = value % HASH_TABLE_SIZE;

    return value;
}

entry_t *ht_pair(const char *key, const char *value)
{
    entry_t *entry = safe_malloc(sizeof(entry_t) * 1);
    entry->key = safe_malloc(strlen(key) + 1);
    entry->value = safe_malloc(strlen(value) + 1);

    strcpy(entry->key, key);
    strcpy(entry->value, value);

    entry->next = NULL;

    return entry;
}

ht_t *ht_create(void)
{
    int i = 0;
    ht_t *hashtable = safe_malloc(sizeof(ht_t) * 1);

    hashtable->entries = safe_malloc(sizeof(entry_t *) * HASH_TABLE_SIZE);

    for (; i < HASH_TABLE_SIZE; ++i)
    {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t *hashtable, const char *key, const char *value)
{
    entry_t *prev;
    unsigned int slot = hash(key);
    entry_t *entry = hashtable->entries[slot];

    if (entry == NULL)
    {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }


    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            free(entry->value);
            entry->value = safe_malloc(strlen(value) + 1);
            strcpy(entry->value, value);
            return;
        }

        prev = entry;
        entry = prev->next;
    }

    prev->next = ht_pair(key, value);
}


char *ht_get(ht_t *hashtable, const char *key)
{


    unsigned int slot = hash(key);

    

    entry_t *entry = hashtable->entries[slot];


    if (entry == NULL)
    {
        return NULL;
    }


    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return entry->value;
        }

        entry = entry->next;
    }

    return NULL;
}

void ht_del(ht_t *hashtable, const char *key)
{
    
    entry_t *prev;
    int idx = 0;
    unsigned int bucket = hash(key);
    entry_t *entry = hashtable->entries[bucket];


    if (entry == NULL)
    {
        return;
    }

    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            if (entry->next == NULL && idx == 0)
            {
                hashtable->entries[bucket] = NULL;
            }

            if (entry->next != NULL && idx == 0)
            {
                hashtable->entries[bucket] = entry->next;
            }

            if (entry->next == NULL && idx != 0)
            {
                prev->next = NULL;
            }

            if (entry->next != NULL && idx != 0)
            {
                prev->next = entry->next;
            }

            free(entry->key);
            free(entry->value);
            free(entry);

            return;
        }

        prev = entry;
        entry = prev->next;

        ++idx;
    }
}

void ht_free(ht_t *hashtable)
{
    int i;
    for (i = 0; i < HASH_TABLE_SIZE; ++i)
    {
   
        entry_t *entry = hashtable->entries[i];


        if (entry == NULL)
        {
            continue;
        }

        while (entry != NULL)
        {
            entry_t *prev = entry;
            entry = prev->next;

            free(prev->key);
            free(prev->value);
            free(prev);
        }
    }

    free(hashtable->entries);
    free(hashtable);
}
