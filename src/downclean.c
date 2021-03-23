/* ***** ***** */

#include <stddef.h>
#include "stack.h"
#include "types.h"
#include "node.h"
#include "uplink.h"
#include "heap.h"
#include "downclean.h"

/* ***** ***** */

static inline
void install_child(struct node nd, struct uplink *lk)
{
    switch (lk->rel) {
    case CHILD_REL:
        single_of_child(lk)->child = nd;
        break;
    case LCHILD_REL:
        branch_of_lchild(lk)->lchild = nd;
        break;
    case RCHILD_REL:
        branch_of_rchild(lk)->rchild = nd;
        break;
    }    
}

void replace_child(struct node nd, struct uplink_dll *lks)
{
    if (!lks->head) { return; }
    struct uplink *lk = head_of(*lks);
    struct uplink *lk_next = next_uplink(lk);
    do {
        install_child(nd, lk);
        lk = lk_next;
        lk_next = next_uplink(lk);
    }
    while (lk_next);
    struct uplink_dll pars = parents_of_node(nd);
    link_uplinks(lk, head_of(pars));
    set_parents_of_node(nd, *lks);
    *lks = empty_dll();
}

static
void delpar(struct node nd, struct uplink *lk)
{
    struct uplink *n = next_uplink(lk);
    struct uplink *p = prev_uplink(lk);
    link_uplinks(p, n);
    lk->next = 0;
    lk->prev = 0;

    if (!p) {
        struct uplink_dll lks;
        lks.head = address_of(n);
        set_parents_of_node(nd, lks);
    }
}

/* ***** ***** */

struct sll {
    struct sll *next;
    struct node current;
};

void downclean(struct node contractum, struct branch *redex)
{
    replace_child(contractum, &redex->parents);
    unlink_uplink(&redex->lchild_uplink);
    unlink_uplink(&redex->rchild_uplink);

    struct sll init = (struct sll) {
        .next = NULL,
        .current = redex->lchild
    };
    struct sll *pinit = &init;
    struct sll **stack = &pinit;
    PUSH(redex->rchild, stack);
    dehalloc_branch(redex);

    struct node nd;

    while (*stack) {

        POP(nd, stack);
        if (is_empty(parents_of_node(nd))) { continue; }
        
        struct single *s;
        struct branch *b;

        switch (kind(nd)) {

        case SINGLE_NODE:
            s = (struct single*)ptr_of(nd.address);
            delpar(s->child, &s->child_uplink);
            PUSH(s->child, stack);
            dehalloc_single(s);
            break;

        case BRANCH_NODE:
            b = (struct branch*)ptr_of(nd.address);
            delpar(b->lchild, &b->lchild_uplink);
            delpar(b->rchild, &b->rchild_uplink);
            PUSH(b->rchild, stack);
            PUSH(b->lchild, stack);
            dehalloc_branch(b);
            break;

        case LEAF_NODE:
            //dehalloc_leaf((struct leaf*)ptr_of(nd.address));
            break;
        }
    }
}

/* ***** ***** */

// end of downclean.c
