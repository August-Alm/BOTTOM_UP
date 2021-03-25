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
    struct uplink *lk = head_of(*lks);
    if (!lk) { return; }
    struct uplink *lk_next = next_uplink(lk);
    while (lk_next) {
        install_child(nd, lk);
        lk = lk_next;
        lk_next = next_uplink(lk);
    }
    install_child(nd, lk);
    struct uplink_dll pars = parents_of_node(nd);
    link_uplinks(lk, head_of(pars));
    set_parents_of_node(nd, *lks);
    *lks = empty_dll();
}

static
void delpar(struct node nd, struct uplink *lk)
{
    if (!lk) { return; }
    struct uplink *n = next_uplink(lk);
    struct uplink *p = prev_uplink(lk);
    link_uplinks(p, n);
    lk->next = 0;
    lk->prev = 0;

    if (!p) {
        struct uplink_dll lks;
        lks.head = n ? address_of(n) : 0;
        printf("lks.head = %d\n", lks.head);
        set_parents_of_node(nd, lks);
    }
}

/* ***** ***** */

#define NODE_STACK_SZ 1024
#define NODE_STACK_SZ1 1023

struct node node_stack[NODE_STACK_SZ];

int top_node_stack = -1;

static inline
struct node pop_node_stack()
{
    if (top_node_stack < 0) { return as_node(NULL); }
    return node_stack[top_node_stack--];
}

static inline
void push_node_stack(struct node nd)
{
    if (top_node_stack == NODE_STACK_SZ1) {
        exit(EXIT_FAILURE);
    }
    node_stack[++top_node_stack] = nd;
}

/* ***** ***** */

void downclean(struct node contractum, struct branch *redex)
{
    replace_child(contractum, &redex->parents);
    unlink_uplink(&redex->lchild_uplink);
    unlink_uplink(&redex->rchild_uplink);

    push_node_stack(redex->rchild);
    dehalloc_branch(redex);

    struct node nd;

    while (top_node_stack != -1) {

        nd = pop_node_stack();
        if (is_empty(parents_of_node(nd))) { continue; }
        
        switch (kind(nd)) {

        case SINGLE_NODE: {
            struct single *s = (struct single*)ptr_of(nd.address);
            delpar(s->child, &s->child_uplink);
            push_node_stack(s->child);
            dehalloc_single(s);
            continue;
        }
        case BRANCH_NODE: {
            struct branch *b = (struct branch*)ptr_of(nd.address);
            delpar(b->lchild, &b->lchild_uplink);
            delpar(b->rchild, &b->rchild_uplink);
            push_node_stack(b->rchild);
            push_node_stack(b->lchild);
            dehalloc_branch(b);
            continue;
        }
        case LEAF_NODE:
            //dehalloc_leaf((struct leaf*)ptr_of(nd.address));
            continue;
        }
    }
}

/* ***** ***** */

// end of downclean.c
