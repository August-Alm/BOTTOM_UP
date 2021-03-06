/* ***** ***** */

#include <stdlib.h>
#include "malcheck.h"
#include "types.h"
#include "node.h"
#include "uplink.h"
#include "reduce.h"
#include "downclean.h"

/* ***** ***** */

struct upcopy_state {
    struct node new_child;
    struct uplink *cclink;
};

#define UPCOPY_STACK_SZ 1024
#define UPCOPY_STACK_SZ1 1023

int top_upcopy_stack = -1;

struct upcopy_state upcopy_stack[UPCOPY_STACK_SZ];

/* ***** ***** */

static
struct uplink *pop_cclink()
{
    if (top_upcopy_stack < 0) { return NULL; }
    struct uplink *cclink = upcopy_stack[top_upcopy_stack].cclink;
    struct uplink *nxt = next_uplink(cclink);
    if (nxt) {
        upcopy_stack[top_upcopy_stack].cclink = nxt;
    }
    else {
        top_upcopy_stack -= 1;
    }
    return cclink;
}

static inline
void goto_pending()
{
    if (!top_upcopy_stack) { exit(EXIT_FAILURE); }
    top_upcopy_stack -= 1;
}

static
void push_or_goto_pending(void *node_ptr, struct uplink_dll lks)
{
    if (is_empty(lks)) {
        goto_pending();
        return;
    }
    if (top_upcopy_stack == UPCOPY_STACK_SZ1) {
        exit(EXIT_FAILURE);
    }
    upcopy_stack[++top_upcopy_stack] = (struct upcopy_state) {
        .new_child = (struct node) { .address = address_of(node_ptr) },
        .cclink = head_of(lks)
    };
}

/* ***** ***** */

static
void upcopy_child(struct node nc, struct uplink *lk)
{
    struct single *s = single_of_child(lk);
    struct leaf *l = (struct leaf*)ptr_of(s->leaf);
    struct leaf *cl = halloc_leaf();
    cl->name = l->name;
    struct single *cs = halloc_single();
    cs->leaf = address_of(cl);
    cs->child = nc;
    prepend(&cs->child_uplink, &cl->parents);
    push_or_goto_pending(cs, s->parents);
    push_or_goto_pending(cl, l->parents);
}

static
void upcopy_lchild(struct node nc, struct uplink *lk)
{
    struct branch *b = branch_of_lchild(lk);
    struct branch *cb;
    if (!b->cache.address) {
        cb = halloc_branch();
        cb->lchild = nc;
        cb->rchild = b->rchild;
        b->cache = (struct node) { .address = address_of(cb) };
        push_or_goto_pending(cb, b->parents);
    }
    else {
        cb = (struct branch*)ptr_of(b->cache.address);
        cb->lchild = nc;
        goto_pending();
    }
}

static
void upcopy_rchild(struct node nc, struct uplink *lk)
{
    struct branch *b = branch_of_rchild(lk);
    struct branch *cb;
    if (!b->cache.address) {
        cb = halloc_branch();
        cb->lchild = b->lchild;
        cb->rchild = nc;
        b->cache = (struct node) { .address = address_of(cb) };
        push_or_goto_pending(cb, b->parents);
    }
    else {
        cb = (struct branch*)ptr_of(b->cache.address);
        cb->rchild = nc;
        goto_pending();
    }
}

/* ***** ***** */

static
void upcopy()
{
     while (top_upcopy_stack != -1) {
    
        struct node nc = upcopy_stack[top_upcopy_stack].new_child;
        struct uplink *lk = pop_cclink();
        
        switch (lk->rel) {

        case CHILD_REL:
            upcopy_child(nc, lk);
             break;

        case LCHILD_REL:
            upcopy_lchild(nc, lk); 
            break;

        case RCHILD_REL:
            upcopy_rchild(nc, lk); 
            break;
        }
    }
}

/* ***** ***** */

#define UPLINK_STACK_SZ 1024
#define UPLINK_STACK_SZ1 1023

struct uplink *uplink_stack[UPLINK_STACK_SZ];

int top_uplink_stack = -1;

static inline
struct uplink *pop_uplink_stack()
{
    if (top_uplink_stack < 0) { return NULL; }
    return uplink_stack[top_uplink_stack--];
}

static inline
void push_uplink_stack(struct uplink *lk)
{
    if (top_uplink_stack == UPLINK_STACK_SZ1) {
        exit(EXIT_FAILURE);
    }
    uplink_stack[++top_uplink_stack] = lk;
}

/* ***** ***** */

static inline
struct branch *cc_branch(struct node lc, struct node rc)
{
    struct branch *cb = halloc_branch();
    cb->lchild = lc;
    cb->rchild = rc;
    return cb;
}

static
struct branch *get_topapp(struct single *s)
{
    struct branch *topbranch = NULL;
    struct node nd = s->child;
    
    while (kind(nd) == SINGLE_NODE) {
        struct single *s = (struct single*)ptr_of(nd.address);
        push_uplink_stack(&s->child_uplink);
        nd = ((struct single*)ptr_of(nd.address))->child;
    }
    if (kind(nd) == BRANCH_NODE) {
        topbranch = (struct branch*)ptr_of(nd.address);
    }
    return topbranch;
}

/* ***** ***** */

struct node reduce(struct branch *redex)
{
    struct node lchild = redex->lchild;
    if (kind(lchild) != SINGLE_NODE) {
        exit(EXIT_FAILURE);
    }
    struct single *lam = (struct single*)ptr_of(lchild.address);

    struct branch *topapp = get_topapp(lam);
    
    struct branch *cc_topapp = cc_branch(topapp->lchild, topapp->rchild);
    topapp->cache = (struct node) { .address = address_of(cc_topapp) };
    
    struct leaf *var = (struct leaf*)ptr_of(lam->leaf);
    push_or_goto_pending(&redex->rchild, var->parents);
    
    upcopy();
    
    struct node ans = (struct node) { .address = address_of(cc_topapp) };
    
    while (top_uplink_stack != -1) {
        struct uplink *lk = pop_uplink_stack();
        struct single *s = single_of_child(lk);
        struct leaf *l = (struct leaf*)ptr_of(s->leaf);
        struct leaf *cl = halloc_leaf();
        cl->name = l->name;
        struct single *cs = halloc_single();
        cs->leaf = address_of(cl);
        cs->child = ans;
        prepend(&cs->child_uplink, &cl->parents);
        push_or_goto_pending(cl, l->parents);
    
        ans = (struct node) { .address = address_of(cs) };
    }
    
    upcopy();
    
    downclean(ans, redex);
    
    return ans;
}

/* ***** ***** */

// end of reduce.c
