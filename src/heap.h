/* ***** ***** */

#ifndef HEAP_H
#define HEAP_H

/* ***** ***** */

#include <stdint.h>
#include "types.h"

/* ***** ***** */

extern int32_t *heap;

/* ***** ***** */

void heap_setup();

void heap_clear();

void heap_free();

/* ***** ***** */

single_t halloc_single();

void dehalloc_single(single_t s);

branch_t halloc_branch();

void dehalloc_branch(branch_t b);

/* ***** ***** */

#endif // HEAP_H