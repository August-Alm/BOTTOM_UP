/* ***** ***** */

#include <stddef.h>
#include "types.h"
#include "uplink.h"
#include "leaf.h"

/* ***** ***** */

void init_leaf(
    struct leaf *l,
    address_t addr)
{
    *l = (struct leaf) {
        .id = (addr << 2) | LEAF_NODE,
        .parents = empty_dll(),
        .name = NULL
    };
}

/* ***** ***** */

// end of leaf.c
