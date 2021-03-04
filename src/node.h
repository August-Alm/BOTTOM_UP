/* ***** ***** */

#ifndef NODE_H
#define NODE_H

/* ***** ***** */

#include "forceinline.h"
#include "types.h"
#include "heap.h"

/* ***** ***** */

FORCEINLINE
enum node_kind kind(struct node nd)
{
    return *(uint32_t*)(ptr_of(nd.address)) & 3;
}

/* ***** ***** */

struct uplink_dll parents_of_node(struct node nd);

void set_parents_of_node(struct node nd, struct uplink_dll lks);

struct node cache_of_node(struct node nd);

void set_cache_of_node(struct node nd, struct node cache);

void add_to_parents(struct uplink *lk, struct node nd);

/* ***** ***** */

#endif // NODE_H
