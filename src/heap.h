/* ***** ***** */

#ifndef HEAP_H
#define HEAP_H

/* ***** ***** */

#include <stdint.h>
#include "types.h"

/* ***** ***** */

extern uint32_t *heap;

inline __attribute__((always_inline))
void *ptr_of(address_t a)
{
    return (void*)(heap + a);
}

inline __attribute__((always_inline))
address_t address_of(void *p)
{
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
