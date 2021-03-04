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

static
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

    while (*stack) {
        struct node nd;
        POP(nd, stack);
        if (parents_of_node(nd).head) { continue; }
        
        struct single *s;
        struct branch *b;

        switch (kind(nd)) {
        case SINGLE_NODE:
            s = (struct single*)ptr_of(nd.address);
            unlink_uplink(&s->child_uplink);
            PUSH(s->child, stack);
            dehalloc_single(s);
            break;
        case BRANCH_NODE:
            b = (struct branch*)ptr_of(nd.address);
            unlink_uplink(&b->lchild_uplink);
            unlink_uplink(&b->rchild_uplink);
            PUSH(b->lchild, stack);
            PUSH(b->rchild, stack);
            dehalloc_branch(b);
            break;
        case LEAF_NODE:
            b = (struct branch*)ptr_of(nd.address);
            break;
        }
    }
}

/* ***** ***** */

// end of downclean.c
