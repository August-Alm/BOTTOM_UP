/* ***** ***** */

#include <stdlib.h>
#include "malcheck.h"
#include "types.h"
#include "node.h"
#include "uplink.h"
#include "reduce.h"
#include "downclean.h"

/* ***** ***** */

struct upcopy_stack {
    struct node new_child;
    struct uplink *cclink;
    struct upcopy_stack *pending;
};

/* ***** ***** */

static
struct uplink *pop_cclink(struct upcopy_stack *st)
{
    struct uplink *cclink = st->cclink;
    struct uplink *nxt = next_uplink(cclink);
    if (nxt) {
        st->cclink = nxt;
    }
    else {
        struct upcopy_stack *tmp = st;
        *st = *st->pending;
        free(tmp);
    }
    return cclink;
}

static
void goto_pending(struct upcopy_stack *st)
{
    if (st) {
        struct upcopy_stack *tmp = st;
        *st = *st->pending;
        free(tmp);
    }
}

static
void push_or_goto_pending
    (void *node_ptr, struct uplink_dll lks, struct upcopy_stack **st)
{
    if (is_empty(lks)) { goto_pending(*st); }
    else {
        struct upcopy_stack *tmp = malloc(sizeof(struct upcopy_stack));
        MALCHECKx(tmp);
        tmp->new_child = (struct node) { .address = address_of(node_ptr) };
        tmp->cclink = head_of(lks);
        tmp->pending = *st;
        *st = tmp;
    }
}

/* ***** ***** */

static
void upcopy_child(struct node nc, struct uplink *lk, struct upcopy_stack *st)
{
    struct single *s = single_of_child(lk);
    struct leaf *l = (struct leaf*)ptr_of(s->leaf);
    struct leaf *cl = halloc_leaf();
    cl->name = l->name;
    struct single *cs = halloc_single();
    cs->leaf = address_of(cl);
    cs->child = nc;
    prepend(&cs->child_uplink, &cl->parents);
    push_or_goto_pending(cs, s->parents, &st);
    push_or_goto_pending(cl, l->parents, &st);
}

static
void upcopy_lchild(struct node nc, struct uplink *lk, struct upcopy_stack *st)
{
    struct branch *b = branch_of_lchild(lk);
    struct branch *cb;
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
}

static
void upcopy_rchild(struct node nc, struct uplink *lk, struct upcopy_stack *st)
{
    struct branch *b = branch_of_rchild(lk);
    struct branch *cb;
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
}

/* ***** ***** */

static
void upcopy(struct upcopy_stack *st)
{
    while (st) {
    
        struct node nc = st->new_child;
        struct uplink *lk = pop_cclink(st);
        
        switch (lk->rel) {

        case CHILD_REL:
            upcopy_child(nc, lk, st);
             break;

        case LCHILD_REL:
            upcopy_lchild(nc, lk, st); 
            break;

        case RCHILD_REL:
            upcopy_rchild(nc, lk, st); 
            break;
        }
    }
}

/* ***** ***** */

#define UPLINK_STACK_SZ 1024
#define UPLINK_STACK_SZ1 1023

struct uplink_stack {
    int top;
    struct uplink *data[UPLINK_STACK_SZ];
};

static inline
struct uplink *pop(struct uplink_stack *stk)
{
    if (stk->top < 0) { return NULL; }
    return stk->data[stk->top--];
}

static inline
void push(struct uplink *lk, struct uplink_stack *stk)
{
    if (stk->top == UPLINK_STACK_SZ1) {
        exit(EXIT_FAILURE);
    }
    stk->data[++stk->top] = lk;
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
struct branch *get_topapp(struct single *s, struct uplink_stack *lks)
{
    struct branch *topbranch = NULL;
    struct node nd = s->child;
    
    while (kind(nd) == SINGLE_NODE) {
        struct single *s = (struct single*)ptr_of(nd.address);
        push(&s->child_uplink, lks);
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
    
    struct upcopy_stack *st = NULL;
    
    struct uplink_stack *child_uplinks = malloc(sizeof(struct uplink_stack));

    struct branch *topapp = get_topapp(lam, child_uplinks);
    
    struct branch *cc_topapp = cc_branch(topapp->lchild, topapp->rchild);
    topapp->cache = (struct node) { .address = address_of(cc_topapp) };
    
    struct leaf *var = (struct leaf*)ptr_of(lam->leaf);
    push_or_goto_pending(&redex->rchild, var->parents, &st);
    
    upcopy(st);
    
    struct node ans = (struct node) { .address = address_of(cc_topapp) };
    
    while (child_uplinks) {
        struct uplink *lk = pop(child_uplinks);
        struct single *s = single_of_child(lk);
        struct leaf *l = (struct leaf*)ptr_of(s->leaf);
        struct leaf *cl = halloc_leaf();
        cl->name = l->name;
        struct single *cs = halloc_single();
        cs->leaf = address_of(cl);
        cs->child = ans;
        prepend(&cs->child_uplink, &cl->parents);
        push_or_goto_pending(cl, l->parents, &st);
    
        ans = (struct node) { .address = address_of(cc_topapp) };
    }
    
    upcopy(st);
    
    downclean(ans, redex);
    
    return ans;
}

/* ***** ***** */

// end of reduce.c
