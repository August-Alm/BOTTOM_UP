/* ***** ***** */

#ifndef LEAF_H
#define LEAF_H

/* ***** ***** */

#include "types.h"
#include "heap.h"
#include "uplink.h"
#include "forceinline.h"

/* ***** ***** */

FORCEINLINE
int32_t get_leaf_id(leaf_t l)
{
    return heap[l] & 3;
}

FORCEINLINE
int32_t get_leaf_name_id(leaf_t l)
{
    return heap[l] >> 2;
}

char *get_leaf_name(leaf_t l);

FORCEINLINE
void set_leaf_name_id(leaf_t l, int32_t nid)
{
    heap[l] = (nid << 2) | LEAF_NODE;
}

FORCEINLINE
uplink_dll_t get_leaf_parents(leaf_t l)
{
    return heap[l + 1];
}

FORCEINLINE
void set_leaf_parents(leaf_t l, uplink_dll_t lks)
{
    heap[l + 1] = get_head(lks);
}

/* ***** ***** */

#endif // LEAF_H
