/* ***** ***** */

#include <stddef.h>
#include "uplink.h"
#include "single.h"

/* ***** ***** */

void init_single(struct single *s, address_t addr)
{
    *s = (struct single) {
        .id = (addr << 2) | SINGLE_NODE,
        .leaf = 0,
        .child = (struct node) { .address = 0 },
        .parents = (struct uplink_dll) { .head = 0 },
        .cache = (struct node) { .address = 0 },
    };
    init_uplink(&s->child_uplink, CHILD_REL);
}

/* ***** ***** */

// end of single.c
