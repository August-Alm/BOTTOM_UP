/* ***** ***** */

#include <stddef.h>
#include <stdbool.h>
#include "types.h"
#include "uplink.h"

/* ***** ***** */

extern
struct single *single_of_child(struct uplink *lk);

extern
struct node node_of_child(struct uplink *lk);

extern
struct branch *branch_of_lchild(struct uplink *lk);

extern
struct node node_of_lchild(struct uplink *lk);

extern
struct branch *branch_of_rchild(struct uplink *lk);

extern
struct node node_of_rchild(struct uplink *lk);

extern
struct uplink_dll empty_dll();

extern
void init_uplink(struct uplink *lk, enum uplink_rel rel);

extern
struct uplink *next_uplink(struct uplink *lk);

extern
struct uplink *prev_uplink(struct uplink *lk);

/* ***** ***** */

extern
struct uplink *head_of(struct uplink_dll lks);

extern
bool is_empty(struct uplink_dll lks);

/* ***** ***** */

void link_uplinks(struct uplink *lk1, struct uplink *lk2)
{
   if (lk1) {
        lk1->next = lk2 ? address_of(lk2) : 0;
   }
   if (lk2) {
        lk2->prev = lk1 ? address_of(lk1) : 0;
   }
}

void unlink_uplink(struct uplink *lk)
{
   struct uplink *n = next_uplink(lk);
   struct uplink *p = prev_uplink(lk);
   link_uplinks(p, n);
   lk->next = 0;
   lk->prev = 0;
}

void prepend(struct uplink *lk, struct uplink_dll *lks)
{
    struct uplink *h = head_of(*lks);
    if (h) {
        link_uplinks(lk, h);
    }
    lks->head = address_of(lk);
}

/* ***** ***** */

// end of uplink.c
