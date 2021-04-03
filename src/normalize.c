/* ***** ***** */

#include <stdio.h>
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
#include "print.h"
struct node normalize_wh(struct node nd)
{
    struct node ans = nd;
    struct branch *b = is_redex(ans);
    while (b) {
        ans = reduce(b);
        fprintf_node(stderr, ans);
        printf("\n");
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
        fprintf(stderr, "%s.\n", __FUNCTION__);
        exit(EXIT_FAILURE);
    }
    return norm_stack[top_norm_stack--];
}

static inline
void push_norm_stack(struct node nd)
{
    if (top_norm_stack == NORM_STACK_SZ1) {
        fprintf(stderr, "%s.\n", __FUNCTION__);
        exit(EXIT_FAILURE);
    }
    norm_stack[++top_norm_stack] = nd;
}

/* ***** ***** */

struct node normalize(struct node nd)
{
    push_norm_stack(nd);

    while (top_norm_stack != -1) {

        nd = pop_norm_stack();
        
        switch (kind(nd)) {
        
        case LEAF_NODE:
            continue;
        
        case SINGLE_NODE: {
            struct single *s = (struct single*)ptr_of(nd.address);
            push_norm_stack(s->child);
            continue;
        }
        case BRANCH_NODE: {
            struct branch *b = (struct branch*)ptr_of(nd.address);
            struct node lchild = b->lchild;
            struct node wh_lchild = normalize_wh(lchild);

            switch (kind(wh_lchild)) {
            case LEAF_NODE:
                push_norm_stack(b->rchild);
                continue;
            case SINGLE_NODE:
                push_norm_stack(reduce(b));
                continue;
            case BRANCH_NODE:
                push_norm_stack(b->rchild);
                push_norm_stack(wh_lchild);
                continue;
            }
            continue;
        }
        }
    }
    return nd;
}

/* ***** ***** */

// end of normalize.c
