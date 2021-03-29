/* ***** ***** */

#include <stddef.h>
#include "types.h"
#include "leaf.h"

/* ***** ***** */

void init_leaf(struct leaf *l, address_t addr)
{
    l->id = (addr << 2) | LEAF_NODE;
    l->parents = (struct uplink_dll) { .head = 0 };
    l->name = NULL;
}

/* ***** ***** */

// end of leaf.c
