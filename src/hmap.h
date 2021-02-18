/* ***** ***** */

#ifndef HMAP_H
#define HMAP_H

/* ***** ***** */

#include <stdint.h>
#include <stdbool.h>
#include "types.h"

/* ***** ***** */

struct hmap;

/* ***** ***** */

struct hmap *hmap_create();

bool hmap_lookup(struct hmap *h, char *key, struct var **val);

void hmap_add(struct hmap *h, char *key, struct var *val);

void hmap_swap_val(struct hmap *h, char *key, struct var *val);

void hmap_clear(struct hmap *h);

void hmap_free(struct hmap *h);

void hmap_show(struct hmap *h);

/* ***** ***** */

#endif /* HMAP_H */
