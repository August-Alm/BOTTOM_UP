/* ***** ***** */

#include <stdlib.h>
#include "malcheck.h"
#include "types.h"
#include "heap.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "name.h"

/* ***** ***** */

/* Forward declaration. We use our own heap and address its
 * contents by a 32 bit offset.
 */
uint32_t *heap;

/* Needed to make inline function in `heap.c` available.
 */
extern
void *ptr_of(address_t a);

extern
address_t address_of(void *p);

/* ***** ***** */

/* Corresponds to a preallocated heap of 15 MB.
 */
#define HEAP_MAX 39321 // 3932160

/* Daft magic numbers. The maximum number of freed nodes
 * of the respective kind at any given time.
 */
#define LEAF_MAX 3932 // 393216 
#define SINGLE_MAX 3932 // 393216 
#define BRANCH_MAX 3932 // 393216

/* ***** ***** */

uint32_t idx_heap;

struct leaf **cleared_leaves;
int top_cleared_leaves;

struct single **cleared_singles;
int top_cleared_singles;

struct branch **cleared_branches;
int top_cleared_branches;

/* ***** ***** */

void heap_setup()
{
    heap = malloc(sizeof(uint32_t) * HEAP_MAX);
    MALCHECKx(heap);
    idx_heap = 0;

    cleared_leaves = malloc(sizeof(address_t) * LEAF_MAX);
    MALCHECKx(cleared_leaves);
    cleared_singles = malloc(sizeof(address_t) * SINGLE_MAX);
    MALCHECKx(cleared_singles);
    cleared_branches = malloc(sizeof(address_t) * BRANCH_MAX);
    MALCHECKx(cleared_branches);

    top_cleared_leaves = -1;
    top_cleared_singles = -1;
    top_cleared_branches = -1;
}

void memory_clear()
{
    idx_heap = 0;
    top_cleared_leaves = -1;
    top_cleared_singles = -1;
    top_cleared_branches = -1;
}

void memory_free()
{
    memory_clear();
    free(heap);
    free(cleared_leaves);
    free(cleared_singles); 
    free(cleared_branches);
}

/* ***** ***** */

struct leaf *halloc_leaf()
{
    if (top_cleared_leaves == -1) {
        address_t addr = idx_heap + 2;
        if (addr < HEAP_MAX - sizeof(struct leaf)) {
            struct leaf *l = (struct leaf*)ptr_of(addr);
            init_leaf(l, addr);
            idx_heap += sizeof(struct leaf);
            return l;
        }
        fprintf(stderr, "`heap` full.\n");
        return NULL;
    }
    struct leaf *l = cleared_leaves[top_cleared_leaves--];
    init_leaf(l, address_of(l));
    return l;
}

void dehalloc_leaf(struct leaf *l)
{
    if (top_cleared_leaves < LEAF_MAX - 1) {
        decref_name(l->name);
        cleared_leaves[++top_cleared_leaves] = l; 
    }
    else {
        fprintf(stderr, "`cleared_leaves` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
}

/* ***** ***** */

struct single *halloc_single()
{
    if (top_cleared_singles == -1) {
        address_t addr = idx_heap + 2;
        if (addr < HEAP_MAX - sizeof(struct single)) {
            struct single *s = (struct single*)(heap + addr);
            init_single(s, addr);
            idx_heap += sizeof(struct single);
            return s;
        }
        fprintf(stderr, "`heap` full.\n");
        return 0;
    }
    struct single *s = cleared_singles[top_cleared_singles--];
    init_single(s, address_of(s));
    return s;
}

void dehalloc_single(struct single *s)
{
    dehalloc_leaf((struct leaf*)ptr_of(s->leaf));
    if (top_cleared_singles < SINGLE_MAX - 1) {
        cleared_singles[++top_cleared_singles] = s; 
    }
    else {
        fprintf(stderr, "`cleared_singles` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
}

/* ***** ***** */

struct branch *halloc_branch()
{
    if (top_cleared_branches == -1) {
        address_t addr = idx_heap + 2;
        if (addr < HEAP_MAX - sizeof(struct branch)) {
            struct branch *b = (struct branch*)(heap + addr);
            init_branch(b, addr);
            idx_heap += sizeof(struct branch);
            return b;
        }
        fprintf(stderr, "`heap` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
    struct branch *b = cleared_branches[top_cleared_branches--];
    init_branch(b, address_of(b));
    return b;
}

void dehalloc_branch(struct branch *b)
{
    if (top_cleared_branches < BRANCH_MAX - 1) {
        cleared_branches[++top_cleared_branches] = b; 
    }
    else {
        fprintf(stderr, "`cleared_branches` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
}


/* ***** ***** */

// end of heap.c
