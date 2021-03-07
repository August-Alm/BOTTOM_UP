/* ***** ***** */

#include "heap.h"
#include "uplink.h"
#include "node.h"

/* ***** ***** */

extern
enum node_kind kind(struct node nd);

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
    case SINGLE_NODE:
        ((struct single*)ptr_of(nd.address))->parents = lks;
    case BRANCH_NODE:
        ((struct branch*)ptr_of(nd.address))->parents = lks;
    }
}

void add_to_parents(struct uplink *lk, struct node nd)
{
    struct uplink_dll pars = parents_of_node(nd);
    prepend(lk, &pars);
}

/* ***** ***** */

// end of node.c
