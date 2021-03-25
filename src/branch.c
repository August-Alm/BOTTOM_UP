/* ***** ***** */

#include <stddef.h>
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
        .lchild_uplink = (struct uplink) {
            .prev = 0,
            .next = 0,
            .rel = LCHILD_REL
        },
        .rchild_uplink = (struct uplink) {
            .prev = 0,
            .next = 0,
            .rel = RCHILD_REL
        }

    };
}

/* ***** ***** */

// end of branch.c
