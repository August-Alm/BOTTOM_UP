/* ***** ***** */

#ifndef CONTEXT_H
#define CONTEXT_H

/* ***** ***** */

#include <stdint.h>
#include <stdbool.h>
#include "types.h"

/* ***** ***** */

bool ctx_init();

bool ctx_lookup(char *key, struct term *val);

void ctx_add(char *key, struct term val);

void ctx_swap(char *key, struct term newval);

bool ctx_remove(char *key);

void ctx_clear();

void ctx_free();

/* ***** ***** */

#endif /* CONTEXT_H */
