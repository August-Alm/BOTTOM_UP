/* ***** ***** */

#ifndef NODE_H
#define NODE_H

/* ***** ***** */

#include "forceinline.h"
#include "types.h"
#include "heap.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"

/* ***** ***** */

FORCEINLINE
enum node_kind get_node_kind(node_t nd)
{
    return heap[nd] & 3;
}

FORCEINLINE
uplink_dll_t get_parents(node_t nd)
{
    switch (get_node_kind(nd)) {
    case LEAF_NODE:
        return get_leaf_parents(nd);
    case SINGLE_NODE:
        return get_single_parents(nd);
    default: // BRANCH_NODE:
        return get_branch_parents(nd);
    }
}

FORCEINLINE
void set_parents(node_t nd, uplink_dll_t lks)
{
    switch (get_node_kind(nd)) {
    case LEAF_NODE:
        set_leaf_parents(nd, lks);
        break;
    case SINGLE_NODE:
        set_single_parents(nd, lks);
        break;
    default: // BRANCH_NODE:
        set_branch_parents(nd, lks);
        break;
    }
}

FORCEINLINE
void init_parents(node_t nd)
{
    init_dll(get_parents(nd));
}

FORCEINLINE
void add_to_parents(uplink_t lk, node_t nd)
{
    append(get_parents(nd), lk);
}

/* ***** ***** */

#endif // NODE_H