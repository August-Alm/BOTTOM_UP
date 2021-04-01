/* ***** ***** */

#ifndef HEAP_H
#define HEAP_H

/* ***** ***** */

#include <stdint.h>
#include "types.h"
#include "forceinline.h"

/* ***** ***** */

extern uint32_t *heap;

FORCEINLINE
void *ptr_of(address_t a)
{
    if (!a) { return 0; }
    return (void*)(heap + a);
}

FORCEINLINE
address_t address_of(void *p)
{
    if (!p) { return 0; }
    return (address_t)((uint32_t*)p - heap);
}

/* ***** ***** */

void heap_setup();

void memory_clear();

void memory_free();

/* ***** ***** */

struct leaf *halloc_leaf();

void dehalloc_leaf(struct leaf *l);

struct single *halloc_single();

void dehalloc_single(struct single *s);

struct branch *halloc_branch();

void dehalloc_branch(struct branch *b);

/* ***** ***** */

#endif // HEAP_H
