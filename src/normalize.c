/* ***** ***** */

#include "malcheck.h"
#include "memory.h"
#include "normalize.h"
#include "uplink.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "node.h"
#include "reduce.h"
#include <stdio.h>
#include <stdlib.h>

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

static inline
int norm_stack_count()
{
    return top_norm_stack + 1;
}

static
void norm_stack_push(bool islch, node_t rt, node_t nd)
{
    if (top_norm_stack < NORM_STACK_MAX - 1) {
        norm_stack[++top_norm_stack] =
            (struct normstate) {.is_lch = islch, .root = rt, .node = nd};
    }
    else {
        fprintf(stderr, "`norm_stack` full.\n");
        memory_free();
        abort();
    }
}

static
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
    branch_t b = is_redex(*nd);
    while (b != -1) {
        *nd = reduce(b);
        b = is_redex(*nd);
    }
}

/* ***** ***** */

void mytest(node_t nd, int iter)
{
    int m;
    switch (get_node_kind(nd)) {
    case LEAF_NODE:
        m = 2;
        break;
    case SINGLE_NODE:
        m = 8;
        break; 
    default:
        m = 11;
        break;
    }
    fprintf(stderr, "\nIteration %i\n", iter);
    for (int i = nd; i < nd + m; ++i) {
        fprintf(stderr, "heap[%i] = %i\n", i, heap[i]);
    }
}

void normalize_iter(node_t *node)
{
    norm_stack_push(false, *node, *node);

    struct normstate ns;
    node_t root;
    node_t nd;

    while (norm_stack_count() > 0) {
        ns = norm_stack_pop();
        root = ns.root;
        nd = ns.node;

        if (!(ns.is_lch)) {
            switch (get_node_kind(nd)) {
            case SINGLE_NODE: 
                norm_stack_push(false, root, get_child(nd));
                break;
            case BRANCH_NODE: {
                node_t lch = get_lchild(nd);
                norm_stack_push(true, root, nd);
                norm_stack_push(false, lch, lch);
                break;
            }
            default: // LEAF_NODE:
                *node = root;
                break;
            }
            continue;
        }
        switch (get_node_kind(*node)) {
        case SINGLE_NODE: {
            node_t red = reduce(nd);
            if (nd == root) norm_stack_push(false, red, red);
            else norm_stack_push(false, root, red);
            break;
        }
        default:
            norm_stack_push(false, root, get_rchild(nd));
            break;
        }
    }
}

/* ***** ***** */

node_t loop(node_t root, node_t nd)
{
    switch (get_node_kind(nd)) {
    case SINGLE_NODE:
        return loop(root, get_child(nd));
    case BRANCH_NODE: {
        node_t lch = get_lchild(nd);
        node_t new = loop(lch, lch);
        switch (get_node_kind(new)) {
        case SINGLE_NODE: {
            node_t red = reduce(nd);
            if (nd == root) return loop(red, red);
            else return loop(root, red);
        }
        default:
            return loop(root, get_rchild(nd));
        }
    }
    default: // LEAF
        return root;
    }
}

void normalize_rec(node_t *nd)
{
    *nd = loop(*nd, *nd);
    /*
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
            break;
        }
    }
    */
}

/* ***** ***** */

void normalize(node_t *nd)
{
    normalize_iter(nd);
}

/* ***** ***** */

// end of normalize.c