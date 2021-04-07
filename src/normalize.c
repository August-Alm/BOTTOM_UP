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

struct node normalize_wh(struct node nd)
{
    struct node ans = nd;
    struct branch *b = is_redex(ans);
    while (b) {
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
#include "print.h"
struct node normalize(struct node nd)
{
    struct node ans = normalize_wh(nd);
    push_norm_stack(ans);

    while (top_norm_stack != -1) {

        nd = pop_norm_stack();

        switch (kind(nd)) {

        case SINGLE_NODE: {
            struct single *s = ptr_of(nd.address);
            printf("case SINGLE_NODE, push: "); fprintf_node(stdout, s->child);printf("\n");
            push_norm_stack(s->child);
            continue;
        }
        case BRANCH_NODE: {
            struct branch *b = ptr_of(nd.address);
            struct node lch = b->lchild;

            switch (kind(lch)) {
            case LEAF_NODE:
                printf("case LEAF<BRANCH, push: "); fprintf_node(stdout, b->rchild);printf("\n");
                push_norm_stack(b->rchild);
                continue;
            case SINGLE_NODE:
                nd = reduce(b);
                printf("case SINGLE<BRANCH, push: "); fprintf_node(stdout, nd);printf("\n");
                push_norm_stack(nd);
                continue;
            case BRANCH_NODE:
                printf("case BRANCH<BRANCH, push: "); fprintf_node(stdout, b->rchild); fprintf_node(stdout, lch); printf("\n");
                push_norm_stack(b->rchild);
                push_norm_stack(lch);
                continue;
            }
            continue;
        }
        case LEAF_NODE:
            continue;
        }
    }
    return ans;
}

/* ***** ***** */

// end of normalize.c
