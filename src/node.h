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

FORCEINLINE
struct node as_node(void *ptr)
{
    return (struct node) { .address = address_of(ptr) };
}

/* ***** ***** */

struct uplink_dll parents_of_node(struct node nd);

void set_parents_of_node(struct node nd, struct uplink_dll lks);

void add_to_parents(struct uplink *lk, struct node nd);

/* ***** ***** */

#endif // NODE_H
