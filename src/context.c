/* ***** ***** */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "basics.h"
#include "context.h"

/* ***** ***** */

#define STRLEN 16
#define DEF_SIZE 393
#define NOTP(s) fprintf(stderr, "%s is not present in context\n", s);

/* ***** ***** */

struct entry {
	char *key;
	struct term val;
	char taken;
};

struct hmap {
	struct entry **buckets;
	int size;
};

/* ***** ***** */

struct hmap *ctx = NULL;

bool ctx_init()
{
	ctx = malloc(sizeof(struct hmap));
	MALCHECKb(ctx);

	ctx->size = DEF_SIZE;
	ctx->buckets = calloc(ctx->size, sizeof(struct entry*));
	MALCHECKb(ctx->buckets);

	for (int i = 0; i < ctx->size; ++i) {
		ctx->buckets[i] = calloc(1, sizeof(struct entry));
		ctx->buckets[i]->taken = 0;
		ctx->buckets[i]->key = NULL;
		ctx->buckets[i]->val = (struct term) { .ptr = NULL };
	}
    return true;
}

/* ***** ***** */

int fast_strcmp(const char *ptr0, const char *ptr1, int len)
{
    if (!ptr0) { return 1; }

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

static uint64_t hash_func(char *key)
{
	uint64_t hashed = 0;
	for (int i = 0; key[i] != 0; ++i) {
		hashed += key[i];
	}
	return hashed % ctx->size;
}

bool ctx_lookup(char *key, struct term *val)
{
    uint64_t index = hash_func(key);
	uint64_t start = index;

	while (ctx->buckets[index]->key && strcmp(ctx->buckets[index]->key, key)) {
		index = (index + 1) % ctx->size;
		if (index == start) {
			return false;
		}
	}
    if (!ctx->buckets[index]->key) {
        return false;
    }
	
    *val = ctx->buckets[index]->val;
	return true;
}

void ctx_swap(char *key, struct term newval)
{
	uint64_t index = hash_func(key);
	uint64_t start = index;
	while (ctx->buckets[index]->key && strcmp(ctx->buckets[index]->key, key)) {
		index = (index + 1) % ctx->size;
		if (index == start) {
			NOTP(key);
			return;
		}
	}
	ctx->buckets[index]->val = newval;
}

void ctx_swap_replkey(char **key, struct term newval)
{
    char *ky = *key;
	uint64_t index = hash_func(ky);
	uint64_t start = index;
	while (ctx->buckets[index]->key && strcmp(ctx->buckets[index]->key, ky)) {
		index = (index + 1) % ctx->size;
		if (index == start) {
			NOTP(ky);
			return;
		}
	}
    free(ky);
    *key = ctx->buckets[index]->key;
	ctx->buckets[index]->val = newval;
}

void ctx_add(char *key, struct term val)
{
	uint64_t index = hash_func(key);
	while (ctx->buckets[index]->taken) {
		index = (index + 1) % ctx->size;
	}
	ctx->buckets[index]->key = key;
	ctx->buckets[index]->val = val;
	ctx->buckets[index]->taken = 1;
}

bool ctx_remove(char *key)
{
    uint64_t index = hash_func(key);
	uint64_t start = index;

	while (ctx->buckets[index]->key && strcmp(ctx->buckets[index]->key, key)){
		index = (index + 1) % ctx->size;
		if (index == start) {
			return false;
		}
	}
    if (!ctx->buckets[index]->key) {
        return false;
    }
	
    free(ctx->buckets[index]->key);
    ctx->buckets[index]->taken = 0;
    ctx->buckets[index]->key = NULL;
    ctx->buckets[index]->val = TERM(NULL);
	return true;
}

void ctx_clear()
{
	for (int i = 0; i < ctx->size; ++i) {
		free(ctx->buckets[i]->key);
        ctx->buckets[i]->taken = 0;
        ctx->buckets[i]->key = NULL;
        ctx->buckets[i]->val = TERM(NULL);
	}
	free(ctx->buckets);
	free(ctx);
}

void ctx_free()
{
	for (int i = 0; i < ctx->size; ++i) {
		free(ctx->buckets[i]->key);
		free(ctx->buckets[i]);
	}
	free(ctx->buckets);
	free(ctx);
}

// For debug.
void ctx_show()
{
	for (int i = 0; i < ctx->size; ++i) {
		printf("%d | %s\n", i, ctx->buckets[i]->key);
	}
}
