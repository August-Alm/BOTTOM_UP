/* ***** ***** */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "memory.h"
#include "malcheck.h"
#include "upcopy.h"
#include "heap.h"
#include "uplink.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "node.h"
#include "freenode.h"

/* ***** ***** */

struct ucarg {
    node_t node;
    uplink_dll_t uplinks;
};

#define UPCOPY_STACK_MAX 65536

struct ucarg *upcopy_stack;

int32_t top_upcopy_stack;

void upcopy_stack_setup()
{
    upcopy_stack = malloc(sizeof(struct ucarg) * UPCOPY_STACK_MAX);
    MALCHECKx(upcopy_stack);
    top_upcopy_stack = -1;
}

void upcopy_stack_free()
{
    free(upcopy_stack);
}

int upcopy_stack_count()
{
    return top_upcopy_stack + 1;
}

void upcopy_stack_push(node_t newch, uplink_dll_t pars)
{
    if (top_upcopy_stack < UPCOPY_STACK_MAX - 1) {
        upcopy_stack[++top_upcopy_stack] =
            (struct ucarg) {.node = newch, .uplinks = pars};
    }
    else {
        fprintf(stderr, "`upcopy_stack` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
}

struct ucarg upcopy_stack_pop()
{
    return upcopy_stack[top_upcopy_stack--];
}

bool upcopy_stack_trypop(node_t *nd, uplink_dll_t *lks)
{
    if (top_upcopy_stack >= 0) {
        struct ucarg uca = upcopy_stack[top_upcopy_stack--];
        *nd = uca.node;
        *lks = uca.uplinks;
        return true;
    }
    return false;
}

/* ***** ***** */

single_t new_single(leaf_t oldvar, node_t body)
{
    single_t s = halloc_single();
    init_single(s, get_leaf_name_id(oldvar));
    set_child(s, body);
    add_to_parents(get_child_uplink(s), body);
    upcopy_stack_push(get_leaf(s), get_leaf_parents(oldvar));
    return s;
}

/* ***** ***** */

branch_t new_branch(node_t func, node_t argm)
{
    branch_t b = halloc_branch();
    set_lchild(b, func);
    set_rchild(b, argm);
    return b;
}

/* ***** ***** */

void upcopy_uplink(node_t newch, uplink_t par)
{
    switch (get_rel(par)) {

    case CHILD_REL: {
        single_t s = get_node(par);
        node_t nd = new_single(get_leaf(s), newch);
        upcopy_stack_push(nd, get_single_parents(s));
        break;
    }
    case LCHILD_REL: {
        branch_t b = get_node(par);
        branch_t cc = get_cache(b);
        if (cc == -1) {
            node_t nd = new_branch(newch, get_rchild(b));
            set_cache(b, nd);
            upcopy_stack_push(nd, get_branch_parents(b));
        }
        else {
            set_lchild(cc, newch);
        }
        break;
    }
    case RCHILD_REL: {
        branch_t b = get_node(par);
        branch_t cc = get_cache(b);
        if (cc == -1) {
            node_t nd = new_branch(newch, get_lchild(b));
            set_cache(b, nd);
            upcopy_stack_push(nd, get_branch_parents(b));
        }
        else {
            set_rchild(cc, newch);
        }
        break;
    }
    }
}

void upcopy()
{
    node_t newch = -1;
    uplink_dll_t pars = -1;
    while (upcopy_stack_trypop(&newch, &pars)) {
        uplink_t lk = get_head(pars);
        if (lk == -1) continue;
        uplink_t nxt = get_next(lk);
        while (lk != -1) {
            upcopy_uplink(newch, lk);
            lk = nxt;
            nxt = get_next(lk);
        }
    }
}

/* ***** ***** */

// end of upcopy.c