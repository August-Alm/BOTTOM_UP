/* ***** ***** */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "name.h"
#include "malcheck.h"
#include "node.h"

/* ***** ***** */

#define DEF_SIZE 3932
#define NOTP(s) fprintf(stderr, "%s is not present in hmap\n", s);

/* ***** ***** */

struct entry {
    char *key;
    uint32_t val;
    char taken;
};

struct hmap {
    struct entry **buckets;
    int size;
};

/* ***** ***** */

static uint64_t hash_func(struct hmap *h, char *key)
{
    uint64_t hashed = 0;
    for (int i = 0; key[i] != 0; ++i) {
        hashed += key[i];
    }
    return hashed % h->size;
}
/* ***** ***** */

struct hmap *names;

void setup_names()
{
    struct hmap *new = malloc(sizeof(struct hmap));
    MALCHECKx(new);

    new->size = DEF_SIZE;
    new->buckets = calloc(new->size, sizeof(struct entry*));
    MALCHECKx(new->buckets);

    for (int i = 0; i < new->size; ++i) {
        new->buckets[i] = calloc(1, sizeof(struct entry));
        new->buckets[i]->taken = 0;
        new->buckets[i]->key = NULL;
        new->buckets[i]->val = 0;
    }
    names = new;
}

/* ***** ***** */

uint64_t names_add(char *key)
{
    uint64_t index = hash_func(names, key);
    while (names->buckets[index]->taken) {
        index = (index + 1) % names->size;
    }
    names->buckets[index]->key = key;
    names->buckets[index]->val = 1;
    names->buckets[index]->taken = 1;
    return index;
}

struct name *get_name(char *str)
{
    uint64_t index = hash_func(names, str);
    uint64_t start = index;

    while (names->buckets[index]->key && strcmp(names->buckets[index]->key, str)) {
        index = (index + 1) % names->size;
        if (index == start) {
            uint64_t idx = names_add(str);
            return (struct name*)&names->buckets[idx]->key;
        }
    }
    if (!names->buckets[index]->key) {
        uint64_t idx = names_add(str);
        return (struct name*)&names->buckets[idx]->key;
    }

    struct name *nam = (struct name*)&names->buckets[index]->key;
    nam->refcnt++;
    free(str);
    return nam;
}

void clear_names()
{
    for (int i = 0; i < names->size; ++i) {
        free(names->buckets[i]->key);
        names->buckets[i]->taken = 0;
        names->buckets[i]->key = NULL;
        names->buckets[i]->val = 0;
    }
    free(names->buckets);
    free(names);
}

void free_names()
{
    for (int i = 0; i < names->size; ++i) {
        free(names->buckets[i]->key);
        free(names->buckets[i]);
    }
    free(names->buckets);
    free(names);
}
