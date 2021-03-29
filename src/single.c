/* ***** ***** */

#include <stddef.h>
#include "single.h"

/* ***** ***** */

void init_single(struct single *s, address_t addr)
{
    s->id = (addr << 2) | SINGLE_NODE;
    s->leaf = 0;
    s->child = (struct node) { .address = 0 };
    s->parents = (struct uplink_dll) { .head = 0 };
    s->child_uplink = (struct uplink) {
        .prev = 0,
        .next = 0,
        .rel = CHILD_REL
    };
}

/* ***** ***** */

// end of single.c
