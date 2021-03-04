/* ***** ***** */

#include <stddef.h>
#include "uplink.h"
#include "single.h"

/* ***** ***** */

void init_branch(struct branch *b, address_t addr)
{
    *b = (struct branch) {
        .id = (addr << 2) | BRANCH_NODE,
        .lchild = (struct node) { .address = 0},
        .rchild = (struct node) { .address = 0 },
        .parents = (struct uplink_dll) { .head = 0 },
        .cache = (struct node) { .address = 0 },
    };
    init_uplink(&b->lchild_uplink, LCHILD_REL);
    init_uplink(&b->rchild_uplink, RCHILD_REL);
}

/* ***** ***** */

// end of branch.c
