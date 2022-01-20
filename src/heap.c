/* ***** ***** */

#include "memory.h"
#include "malcheck.h"
#include "heap.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* ***** ***** */

/* Forward declaration. We use our own heap and address its
 * contents by a 32 bit offset.
 */
int32_t *heap;

/* ***** ***** */

#define HEAP_MAX 1073751824 // 2^30

/* Daft magic numbers. The maximum number of freed nodes
 * of the respective kind at any given time.
 */
#define SINGLE_MAX 1048576
#define BRANCH_MAX 1048576

/* ***** ***** */

int32_t idx_heap;

single_t *cleared_singles;
int top_cleared_singles;

branch_t *cleared_branches;
int top_cleared_branches;

/* ***** ***** */

void heap_setup()
{
    heap = malloc(sizeof(int32_t) * HEAP_MAX);
    MALCHECKx(heap);
    idx_heap = 0;

    cleared_singles = malloc(sizeof(address_t) * SINGLE_MAX);
    MALCHECKx(cleared_singles);
    cleared_branches = malloc(sizeof(address_t) * BRANCH_MAX);
    MALCHECKx(cleared_branches);

    top_cleared_singles = -1;
    top_cleared_branches = -1;
}

void heap_clear()
{
    idx_heap = 0;
    top_cleared_singles = -1;
    top_cleared_branches = -1;
}

void heap_free()
{
    heap_clear();
    free(heap);
    free(cleared_singles); 
    free(cleared_branches);
}

/* ***** ***** */

struct single single_init = {
    .id = SINGLE_NODE,
    .leaf = {
        .id = LEAF_NODE,
        .parents = -1
    },
    .child = -1,
    .child_uplink = {
        .rel = CHILD_REL,
        .next = -1,
        .prev = -1
    },
    .parents = -1
};

single_t halloc_single()
{
    if (top_cleared_singles == -1) {
        single_t s = idx_heap;
        if (s < HEAP_MAX - 8) {
            struct single *sptr = heap + s;
            *sptr = single_init;
            idx_heap += 8;
            return s;
        }
        fprintf(stderr, "`heap` full.\n");
        return -1;
    }
    return cleared_singles[top_cleared_singles--];
}

void dehalloc_single(single_t s)
{
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

struct branch branch_init = {
    .id = BRANCH_NODE,
    .lchild = -1,
    .rchild = -1,
    .lchild_uplink = {
        .rel = LCHILD_REL,
        .next = -1,
        .prev = -1
    },
    .rchild_uplink = {
        .rel = RCHILD_REL,
        .next = -1,
        .prev = -1
    },
    .parents = -1,
    .cache = -1
};

branch_t halloc_branch()
{
    if (top_cleared_branches == -1) {
        branch_t b = idx_heap;
        if (b < HEAP_MAX - 11) {
            struct branch *bptr = heap + b;
            *bptr = branch_init;
            idx_heap += 11;
            return b;
        }
        fprintf(stderr, "`heap` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
    return cleared_branches[top_cleared_branches--];
}

void dehalloc_branch(branch_t b)
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