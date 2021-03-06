/* ***** ***** */

#include <stdlib.h>
#include "malcheck.h"
#include "types.h"
#include "uplink.h"
#include "upcopy.h"

/* ***** ***** */

struct upcopy_state {
    struct node new_child;
    struct uplink *cclink;
    struct upcopy_state *pending;
};

/* ***** ***** */

static
struct uplink *pop_cclink(struct upcopy_state *st)
{
    struct uplink *cclink = st->cclink;
    struct uplink *nxt = next_uplink(cclink);
    if (nxt) {
        st->cclink = nxt;
    }
    else {
        struct upcopy_state *tmp = st;
        *st = *st->pending;
        free(tmp);
    }
    return cclink;
}

static
void *goto_pending(struct upcopy_state *st)
{
    if (!st) { return NULL; }
    struct upcopy_state *tmp = st;
    *st = *st->pending;
    free(tmp);
}

static
void push_or_goto_pending
    (void *node_ptr, struct uplink_dll lks, struct upcopy_state **st)
{
    if (is_empty(lks)) { goto_pending(*st); }
    else {
        struct upcopy_state *tmp = malloc(sizeof(struct upcopy_state));
        MALCHECKx(tmp);
        tmp->new_child = (struct node) { .address = address_of(node_ptr) };
        tmp->cclink = head_of(lks);
        tmp->pending = *st;
        *st = tmp;
    }
}

/* ***** ***** */

void upcopy(struct node new_child, struct uplink *cclink)
{
    struct upcopy_state init = (struct upcopy_state) {
        .new_child = new_child,
        .cclink = cclink,
        .pending = NULL
    };
    struct upcopy_state *st = &init;

    while (st) {
    
        struct node nc = st->new_child;
        struct uplink *lk = pop_cclink(st);
        
        struct leaf *l, *cl;
        struct single *s, *cs;
        struct branch *b, *cb;

        switch (lk->rel) {

        case CHILD_REL:
            s = single_of_child(lk);
            l = (struct leaf*)ptr_of(s->leaf);
            cl = halloc_leaf();
            cl->name = l->name;
            cs = halloc_single();
            cs->leaf = address_of(cl);
            cs->child = nc;
            prepend(&cs->child_uplink, &cl->parents);
            push_or_goto_pending(cs, s->parents, &st);
            push_or_goto_pending(cl, l->parents, &st);
            break;

        case LCHILD_REL:
            b = branch_of_lchild(lk);
            if (!b->cache.address) {
                cb = halloc_branch();
                cb->lchild = nc;
                cb->rchild = b->rchild;
                b->cache = (struct node) { .address = address_of(cb) };
                push_or_goto_pending(cb, b->parents, &st);
            }
            else {
                cb = (struct branch*)ptr_of(b->cache.address);
                cb->lchild = nc;
                goto_pending(st);
            }
            break;

        case RCHILD_REL:
            b = branch_of_rchild(lk);
            if (!b->cache.address) {
                cb = halloc_branch();
                cb->lchild = b->lchild;
                cb->rchild = nc;
                b->cache = (struct node) { .address = address_of(cb) };
                push_or_goto_pending(cb, b->parents, &st);
            }
            else {
                cb = (struct branch*)ptr_of(b->cache.address);
                cb->rchild = nc;
                goto_pending(st);
            }
            break;
        }
    }
}

/* ***** ***** */

// end of upcopy.c
