/* ***** ***** */

#ifndef UPLINK_H
#define UPLINK_H

/* ***** ***** */

#include <stddef.h>
#include "containerof.h"
#include "forceinline.h"
#include "types.h"
#include "heap.h"

/* ***** ***** */

FORCEINLINE
struct single *single_of_child(struct uplink *lk)
{
    return CONTAINEROF(lk, struct single, child_uplink);
}

FORCEINLINE
struct node node_of_child(struct uplink *lk)
{
    struct single *s = CONTAINEROF(lk, struct single, child_uplink);
    return (struct node) { .address = (s->id) << 2 };
}

FORCEINLINE
struct branch *branch_of_lchild(struct uplink *lk)
{
    return CONTAINEROF(lk, struct branch, lchild_uplink);
}

FORCEINLINE
struct node node_of_lchild(struct uplink *lk)
{
    struct branch *b = CONTAINEROF(lk, struct branch, lchild_uplink);
    return (struct node) { .address = (b->id) << 2 };
}

FORCEINLINE
struct branch *branch_of_rchild(struct uplink *lk)
{
    return CONTAINEROF(lk, struct branch, rchild_uplink);
}

FORCEINLINE
struct node node_of_rchild(struct uplink *lk)
{
    struct branch *b = CONTAINEROF(lk, struct branch, rchild_uplink);
    return (struct node) { .address = (b->id) << 2 };
}

/* ***** ***** */

FORCEINLINE
struct uplink_dll empty_dll()
{
    return (struct uplink_dll) { .head = 0 };
}

FORCEINLINE
void init_uplink(struct uplink *lk, enum uplink_rel rel)
{
    *lk = (struct uplink) {
        .prev = 0,
        .next = 0,
        .rel = rel
    };
}

FORCEINLINE
struct uplink *next_uplink(struct uplink *lk)
{
    address_t n = lk->next;
    if (n) { return (struct uplink*)ptr_of(n); }
    return NULL;
}

FORCEINLINE
struct uplink *prev_uplink(struct uplink *lk)
{
    address_t p = lk->prev;
    if (p) { return (struct uplink*)ptr_of(p); }
    return NULL;
}

/* ***** ***** */

FORCEINLINE
struct uplink *head_of(struct uplink_dll lks)
{
    return (struct uplink*)ptr_of(lks.head);
}

/* ***** ***** */

void link_uplinks(struct uplink *lk1, struct uplink *lk2);

void unlink_uplink(struct uplink *lk);

void prepend(struct uplink *lk, struct uplink_dll *lks);

/* ***** ***** */

#endif // UPLINK_H
