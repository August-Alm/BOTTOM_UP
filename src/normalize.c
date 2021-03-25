/* ***** ***** */

#include <stdlib.h>
#include "node.h"
#include "normalize.h"

/* ***** ***** */

static inline
struct branch *is_redex(struct node nd)
{
    if (!nd.address) { return NULL; }
    if (kind(nd) != BRANCH_NODE) {
        return NULL;
    }
    struct branch *b = (struct branch*)ptr_of(nd.address);
    struct node lchild = b->lchild;
    if (kind(lchild) == SINGLE_NODE) {
        return b;
    }
    return NULL;
}

/* ***** ***** */

struct node normalize_wh(struct node nd)
{
    struct node ans = nd;
    struct branch *b = is_redex(ans);
    while (b) {
        fprintf(stderr, "reduce once\n");
        ans = reduce(b);
        b = is_redex(ans);
    }
    return ans;
}

/* ***** ***** */

#define NORM_STACK_SZ 512
#define NORM_STACK_SZ1 511

int top_norm_stack = -1;

struct node norm_stack[NORM_STACK_SZ];

static inline
struct node pop_norm_stack()
{
    if (top_norm_stack == -1) {
        exit(EXIT_FAILURE);
    }
    return norm_stack[top_norm_stack--];
}

static inline
void push_norm_stack(struct node nd)
{
    if (top_norm_stack == NORM_STACK_SZ1) {
        exit(EXIT_FAILURE);
    }
    norm_stack[++top_norm_stack] = nd;
}

/* ***** ***** */

struct node normalize(struct node nd)
{
    push_norm_stack(nd);
    while (top_norm_stack != -1) {
        struct node nd = pop_norm_stack();
        
        switch (kind(nd)) {
        
        case LEAF_NODE:
            break;
        
        case SINGLE_NODE: {
            struct single *s = (struct single*)ptr_of(nd.address);
            push_norm_stack(s->child);
            break;
        }
        case BRANCH_NODE: {
            struct branch *b = (struct branch*)ptr_of(nd.address);
            struct node lchild = b->lchild;
            struct node wh_lchild = normalize_wh(lchild);
        
            switch (kind(wh_lchild)) {
            case LEAF_NODE:
                push_norm_stack(b->rchild);
                break;
            case SINGLE_NODE:
                push_norm_stack(reduce(b));
                break;
            case BRANCH_NODE:
                push_norm_stack(b->rchild);
                push_norm_stack(wh_lchild);
                break;
            }
            break;
        }
        }
    }
    return nd;
}

/* ***** ***** */

// end of normalize.c
