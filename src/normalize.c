/* ***** ***** */

#include <stdio.h>
#include <stdlib.h>
#include "malcheck.h"
#include "normalize.h"
#include "uplink.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "node.h"
#include "reduce.h"

/* ***** ***** */

struct normstate {
    bool is_lch;
    node_t root;
    node_t node;
};

#define NORM_STACK_MAX 65536

struct normstate *norm_stack;

int32_t top_norm_stack;

void norm_stack_setup()
{
    norm_stack = malloc(sizeof(struct normstate) * NORM_STACK_MAX);
    MALCHECKx(norm_stack);
    top_norm_stack = -1;
}

void norm_stack_free()
{
    free(norm_stack);
}

int norm_stack_count()
{
    return top_norm_stack + 1;
}

void norm_stack_push(bool islch, node_t rt, node_t nd)
{
    if (top_norm_stack < NORM_STACK_MAX - 1) {
        norm_stack[++top_norm_stack] =
            (struct normstate) {.is_lch = islch, .root = rt, .node = nd};
    }
    else {
        fprintf(stderr, "`norm_stack` full.\n");
        free(norm_stack);
        memory_free();
        exit(EXIT_FAILURE);
    }
}

struct normstate norm_stack_pop()
{
    return norm_stack[top_norm_stack--];
}

bool norm_stack_trypop(bool *islch, node_t *rt, node_t *nd)
{
    if (top_norm_stack >= 0) {
        struct normstate ns = norm_stack[top_norm_stack--];
        *islch = ns.is_lch; 
        *rt = ns.root;
        *nd = ns.node;
        return true;
    }
    return false;
}

/* ***** ***** */

static inline
branch_t is_redex(node_t nd)
{
    if (get_node_kind(nd) != BRANCH_NODE) return -1;
    if (get_node_kind(get_lchild(nd)) != SINGLE_NODE) return -1;
    return nd;
}

/* ***** ***** */

void normalize_wh(node_t *nd)
{
    node_t res = *nd;
    branch_t b = is_redex(res);
    while (b != -1) {
        res = reduce(b);
        b = is_redex(res);
    }
    *nd = res;
}

/* ***** ***** */

void normalize_iter(node_t *nd)
{
    bool is_lch = false;
    node_t root = *nd;
    node_t node = *nd;
    norm_stack_push(is_lch, root, node);

    while (norm_stack_trypop(&is_lch, &root, &node)) {

        if (!is_lch) {
            switch (get_node_kind(node)) {
            case LEAF_NODE:
                *nd = root;
                continue;
            case SINGLE_NODE: {
                node_t ch = get_child(node);
                norm_stack_push(false, root, ch);
                continue;
            }
            case BRANCH_NODE: {
                node_t lch = get_lchild(node);
                norm_stack_push(true, root, node);
                norm_stack_push(false, lch, lch);
                continue;
            }
            }
        }
        if (get_node_kind(*nd) == SINGLE_NODE) {
            node_t red = reduce(node);
            if (node == root) {
                norm_stack_push(false, red, red);
            }
            else {
                norm_stack_push(false, root, red);
            }
        }
        else {
            node_t rch = get_rchild(node);
            norm_stack_push(false, root, rch);
        }
    }
}

/* ***** ***** */

void normalize_rec(node_t *nd)
{
    switch (get_node_kind(*nd)) {
        case LEAF_NODE:
            return;
        case SINGLE_NODE: {
            node_t ch = get_child(*nd);
            normalize_rec(&ch);
            break;
        }
        case BRANCH_NODE : {
            branch_t b = *nd;
            node_t lch = get_lchild(b);
            normalize_rec(&lch);
            if (get_node_kind(lch) == SINGLE_NODE) {
                *nd = reduce(b);
                normalize_rec(nd);
            }
            else {
                node_t rch = get_rchild(b);
                normalize_rec(&rch);
            }
        }
    }
}

/* ***** ***** */

void normalize(node_t *nd)
{
    normalize_iter(nd);
}

/* ***** ***** */

// end of normalize.c