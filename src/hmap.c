/* ***** ***** */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hmap.h"
#include "basics.h"

/* ***** ***** */

#define STRLEN 32
#define DEF_SIZE 3932
#define NOTP(s) fprintf(stderr, "%s is not present in hmap\n", s);

/* ***** ***** */

struct entry {
	char *key;
	struct var *val;
	char taken;
};

struct hmap {
	struct entry **buckets;
	int size;
};

/* ***** ***** */

int fast_strcmp(const char *ptr0, const char *ptr1, int len)
{
    int fast = len / sizeof(size_t) + 1;
    int val = (fast - 1) * sizeof(size_t);
    int current_block = 0;

    if (len <= sizeof(size_t)) { fast = 0; }

    size_t *lptr0 = (size_t*)ptr0;
    size_t *lptr1 = (size_t*)ptr1;

    while (current_block < fast) {
        if ((lptr0[current_block] ^ lptr1[current_block])) {
            int pos;
            for (pos = current_block * sizeof(size_t); pos < len ; ++pos) {
                if ((ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0)) {
                    return (int)((uint8_t)ptr0[pos] - (uint8_t)ptr1[pos]);
                }
            }
        }
        ++current_block;
    }
    while (len > val) {
        if ((ptr0[val] ^ ptr1[val])) { 
            return (int)((uint8_t)ptr0[val] - (uint8_t)ptr1[val]); 
        }
        ++val;
    }
    return 0;
}

/* ***** ***** */

static uint64_t hash_func(struct hmap *h, char *key)
{
	uint64_t hashed = 0;
	for (int i = 0; key[i] != 0; ++i) {
		hashed += key[i];
	}
	return hashed % h->size;
}

bool hmap_lookup(struct hmap *h, char *key, struct var **val)
{
	uint64_t index = hash_func(h, key);
	uint64_t start = index;

	while (!fast_strcmp(h->buckets[index]->key, key, STRLEN)){
		index = (index + 1) % h->size;
		if (index == start) {
			return false;
		}
	}
	*val = h->buckets[index]->val;
	return true;
}

void hmap_swap_val(struct hmap *h, char *key, struct var *new)
{
	uint64_t index = hash_func(h, key);
	uint64_t start = index;
	while (!fast_strcmp(h->buckets[index]->key, key, STRLEN)) {
		index = (index + 1) % h->size;
		if (index == start) {
			NOTP(key);
			return;
		}
	}
	h->buckets[index]->val = new;
}

void hmap_add(struct hmap *h, char *key, struct var *val)
{
	uint64_t index = hash_func(h, key);
	while (h->buckets[index]->taken) {
		index = (index + 1) % h->size;
	}
	h->buckets[index]->key = key;
	h->buckets[index]->val = val;
	h->buckets[index]->taken = 1;
}

struct hmap *hmap_create()
{
	struct hmap *new = malloc(sizeof(struct hmap));
	MALCHECK(new);

	new->size = DEF_SIZE;
	new->buckets = calloc(new->size, sizeof(struct entry*));
	MALCHECK(new->buckets);

	for (int i = 0; i < new->size; ++i) {
		new->buckets[i] = calloc(1, sizeof(struct entry));
		new->buckets[i]->taken = 0;
		new->buckets[i]->key = NULL;
		new->buckets[i]->val = NULL;
	}
	return new;
}

void hmap_clear(struct hmap *h)
{
	for (int i = 0; i < h->size; ++i) {
		free(h->buckets[i]->key);
        h->buckets[i]->taken = 0;
        h->buckets[i]->key = NULL;
        h->buckets[i]->val = NULL;
	}
	free(h->buckets);
	free(h);
}

void hmap_free(struct hmap *h)
{
	for (int i = 0; i < h->size; ++i) {
		free(h->buckets[i]->key);
		free(h->buckets[i]);
	}
	free(h->buckets);
	free(h);
}

void hmap_show(struct hmap *h)
{
	for (int i = 0; i < h->size; ++i) {
		printf("%d | %s\n", i, h->buckets[i]->key);
	}
}
