/* ***** ***** */

#include "heap.h"
#include "uplink.h"
#include "node.h"

/* ***** ***** */

extern
enum node_kind kind(struct node nd);

extern
struct node as_node(void *ptr);

/* ***** ***** */

struct uplink_dll parents_of_node(struct node nd)
{
    switch (kind(nd)) {
    case LEAF_NODE:
        return ((struct leaf*)ptr_of(nd.address))->parents;
    case SINGLE_NODE:
        return ((struct single*)ptr_of(nd.address))->parents;
    case BRANCH_NODE:
        return ((struct branch*)ptr_of(nd.address))->parents;
    }
}

void set_parents_of_node(struct node nd, struct uplink_dll lks)
{
    switch (kind(nd)) {
    case LEAF_NODE:
        ((struct leaf*)ptr_of(nd.address))->parents = lks;
        break;
    case SINGLE_NODE:
        ((struct single*)ptr_of(nd.address))->parents = lks;
        break;
    case BRANCH_NODE:
        ((struct branch*)ptr_of(nd.address))->parents = lks;
        break;
    }
}

void add_to_parents(struct uplink *lk, struct node nd)
{
    void *nd_ptr = ptr_of(nd.address);
    if (!nd.address) { return; }
    switch (kind(nd)) {
    case LEAF_NODE: 
        prepend(lk, &(((struct leaf*)nd_ptr)->parents));
        break;
    case SINGLE_NODE:
        prepend(lk, &(((struct single*)nd_ptr)->parents));
        break;
    case BRANCH_NODE:
        prepend(lk, &(((struct branch*)nd_ptr)->parents));
        break;
    }
}

/* ***** ***** */

// end of node.c
