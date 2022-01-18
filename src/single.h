/* ***** ***** */

#ifndef SINGLE_H
#define SINGLE_H

/* ***** ***** */

#include "types.h"
#include "heap.h"
#include "uplink.h"
#include "leaf.h"
#include "forceinline.h"

/* ***** ***** */

FORCEINLINE
int32_t get_single_id(single_t s)
{
    return heap[s];
}

FORCEINLINE
leaf_t get_leaf(single_t s)
{
    return heap[s + 1];
}

FORCEINLINE
node_t get_child(single_t s)
{
    return heap[s + 3];
}

FORCEINLINE
void set_child(single_t s, node_t ch)
{
    heap[s + 3] = ch;
}

FORCEINLINE
uplink_t get_child_uplink(single_t s)
{
    return heap[s + 4];
}

FORCEINLINE
uplink_dll_t get_single_parents(single_t s)
{
    return heap[s + 7];
}

FORCEINLINE
void set_single_parents(single_t s, uplink_dll_t lks)
{
    heap[s + 7] = get_head(lks);
}

FORCEINLINE
void init_single(single_t s, int32_t nid)
{
    set_leaf_name_id(get_leaf(s), nid);
}

/* ***** ***** */

#endif // SINGLE_H
