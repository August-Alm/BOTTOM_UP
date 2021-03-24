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
    struct uplink *lk = head_of(lks);
    if (!lk) {
        goto_pending();
        return;
    }
    if (top_upcopy_stack == UPCOPY_STACK_SZ1) {
        exit(EXIT_FAILURE);
    }
    upcopy_stack[++top_upcopy_stack] = (struct upcopy_state) {
        .new_child = as_node(node_ptr),
        .cclink = lk
    };
}

/* ***** ***** */

static
void upcopy_child(struct node nc, struct uplink *lk)
{
    struct single *s = single_of_child(lk);
    struct leaf *l = (struct leaf*)ptr_of(s->leaf);
    struct leaf *cl = halloc_leaf();
    struct name *nam = l->name;
    cl->name = nam;
    nam->refcnt++;
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
        b->cache = as_node(cb);
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
     while (top_upcopy_stack >= 0) {
    
        struct node nc = upcopy_stack[top_upcopy_stack].new_child;
        struct uplink *lk = pop_cclink();
        
        switch (lk->rel) {

        case CHILD_REL:
            fprintf(stderr, "upcopy child_rel\n");
            upcopy_child(nc, lk);
            continue;

        case LCHILD_REL:
            fprintf(stderr, "upcopy lchild_rel\n");
            upcopy_lchild(nc, lk); 
            continue;

        case RCHILD_REL:
            fprintf(stderr, "upcopy rchild_rel\n");
            upcopy_rchild(nc, lk); 
            continue;
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

static
int is_length_one(struct uplink_dll lks)
{
    struct uplink *head = head_of(lks);
    if (!head) { return 0; }
    struct uplink *next = next_uplink(head);
    if (!next) { return 1; }
    return 0;
}

static inline
struct branch *cc_branch(struct node lc, struct node rc)
{
    struct branch *cb = halloc_branch();
    cb->lchild = lc;
    cb->rchild = rc;
    return cb;
}

static
struct node get_topnode(struct single *s)
{
    struct node topnode = s->child;
    
    while (kind(topnode) == SINGLE_NODE) {
        struct single *si = (struct single*)ptr_of(topnode.address);
        push_uplink_stack(&si->child_uplink);
        topnode = si->child;
    }
    return topnode;
}

/* ***** ***** */

struct node reduce(struct branch *redex)
{
    struct node lchild = redex->lchild;
    if (kind(lchild) != SINGLE_NODE) {
        exit(EXIT_FAILURE);
    }
    struct single *lam = (struct single*)ptr_of(lchild.address);
    struct leaf *var = (struct leaf*)ptr_of(lam->leaf);
    struct uplink_dll varpars = var->parents;

    struct node ans;

    if (is_empty(varpars)) {
        ans = lam->child;
        downclean(ans, redex);
        return ans;
    }

    if (is_length_one(lam->parents)) {
        replace_child(redex->rchild, &varpars);
        ans = lam->child;
        downclean(ans, redex);
        return ans;
    }
    
    struct node topnode = get_topnode(lam);
    if (kind(topnode) == BRANCH_NODE) {
        struct branch *topapp = (struct branch*)ptr_of(topnode.address);
        struct branch *cc_topapp = cc_branch(topapp->lchild, topapp->rchild);
        topapp->cache = as_node(cc_topapp);
        ans = as_node(cc_topapp);
    }
    else {
        ans = redex->rchild;
    }
    push_or_goto_pending(&redex->rchild, var->parents);
    
    upcopy();
    
    while (top_uplink_stack != -1) {
        struct uplink *lk = pop_uplink_stack();
        struct single *s = single_of_child(lk);
        struct leaf *l = (struct leaf*)ptr_of(s->leaf);
        struct leaf *cl = halloc_leaf();
        struct name *nam = l->name;
        cl->name = nam;
        nam->refcnt++;
        struct single *cs = halloc_single();
        cs->leaf = address_of(cl);
        cs->child = ans;
        prepend(&cs->child_uplink, &cl->parents);
        push_or_goto_pending(cl, l->parents);
    
        ans = as_node(cs);
    }
    
    upcopy();
    
    downclean(ans, redex);
    
    return ans;
}

/* ***** ***** */

// end of reduce.c
