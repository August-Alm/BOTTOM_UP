/* ***** ***** */

#ifndef BRANCH_H
#define BRANCH_H

/* ***** ***** */

#include "types.h"
#include "heap.h"
#include "uplink.h"
#include "forceinline.h"

/* ***** ***** */

FORCEINLINE
node_t get_lchild(branch_t b)
{
    return heap[b + 1];
}

FORCEINLINE
void set_lchild(branch_t b, node_t lch)
{
    heap[b + 1] = lch;
}

FORCEINLINE
node_t get_rchild(branch_t b)
{
    return heap[b + 2];
}

FORCEINLINE
void set_rchild(branch_t b, node_t rch)
{
    heap[b + 2] = rch;
}

FORCEINLINE
uplink_t get_lchild_uplink(branch_t b)
{
    return heap[b + 3];
}

FORCEINLINE
uplink_t get_rchild_uplink(branch_t b)
{
    return heap[b + 6];
}

FORCEINLINE
uplink_dll_t get_branch_parents(branch_t b)
{
    return heap[b + 9];
}

FORCEINLINE
uplink_dll_t set_branch_parents(branch_t b, uplink_dll_t lks)
{
    heap[b + 9] = get_head(lks);
}

FORCEINLINE
branch_t get_cache(branch_t b)
{
    return heap[b + 10];
}

FORCEINLINE
void set_cache(branch_t b, branch_t cc)
{
    heap[b + 10] = cc;
}

FORCEINLINE
void clear_cache(branch_t b)
{
    heap[b + 10] = -1;
}

/* ***** ***** */

#endif // BRANCH_H
