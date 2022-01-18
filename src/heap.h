/* ***** ***** */

#ifndef HEAP_H
#define HEAP_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

extern uint32_t *heap;

/* ***** ***** */

void heap_setup();

void memory_clear();

void memory_free();

/* ***** ***** */

single_t halloc_single();

void dehalloc_single(single_t s);

branch_t halloc_branch();

void dehalloc_branch(branch_t b);

/* ***** ***** */

#endif // HEAP_H