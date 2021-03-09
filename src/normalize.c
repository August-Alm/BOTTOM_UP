/* ***** ***** */

#include <stdlib.h>
#include "node.h"
#include "normalize.h"

/* ***** ***** */

#define NORM_WH_STACK_SZ 512
#define NORM_WH_STACK_SZ1 511

int top_norm_wh_stack = -1;

struct node norm_wh_stack[NORM_WH_STACK_SZ];

static inline
struct node pop_normalization_wh_stack()
{
    if (top_norm_wh_stack == -1) {
        exit(EXIT_FAILURE);
    }
    return norm_wh_stack[top_norm_wh_stack--];
}

static inline
void push_norm_wh_stack(struct node nd)
{
    if (top_norm_wh_stack == NORM_WH_STACK_SZ1) {
        exit(EXIT_FAILURE);
    }
    norm_wh_stack[++top_norm_wh_stack] = nd;
}

static inline
struct branch *is_redex(struct node nd)
{
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
    struct branch *b = is_redex(nd);
    if (!b) {
        return ans;
    }
    push_norm_wh_stack(ans);

    while (top_norm_wh_stack != -1) {
        ans = reduce(b);
        b = is_redex(ans);
        if (b) { push_norm_wh_stack(ans); }
    }
    return ans;
}

/* ***** ***** */

/* ***** ***** */

/* ***** ***** */

// end of normalize.c
