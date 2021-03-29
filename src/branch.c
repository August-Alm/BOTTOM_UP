/* ***** ***** */

#include <stddef.h>
#include "single.h"

/* ***** ***** */

void init_branch(struct branch *b, address_t addr)
{
     b->id = (addr << 2) | BRANCH_NODE;
     b->lchild = (struct node) { .address = 0 };
     b->rchild = (struct node) { .address = 0 };
     b->parents = (struct uplink_dll) { .head = 0 };
     b->cache = (struct node) { .address = 0 };
     b->lchild_uplink = (struct uplink) {
         .prev = 0,
         .next = 0,
         .rel = LCHILD_REL
     };
     b->rchild_uplink = (struct uplink) {
         .prev = 0,
         .next = 0,
         .rel = RCHILD_REL
     };
}

/* ***** ***** */

// end of branch.c
