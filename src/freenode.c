/* ***** ***** */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freenode.h"
#include "malcheck.h"
#include "heap.h"
#include "uplink.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "node.h"

/* ***** ***** */

#define CLEANUP_STACK_MAX 65536

uplink_dll_t *cleanup_stack;

int32_t top_cleanup_stack;

void cleanup_stack_setup()
{
    cleanup_stack = malloc(sizeof(uplink_dll_t) * CLEANUP_STACK_MAX);
    MALCHECKx(cleanup_stack);
    top_cleanup_stack = -1;
}

void cleanup_stack_free()
{
    free(cleanup_stack);
}

int cleanup_stack_count()
{
    return top_cleanup_stack + 1;
}

void cleanup_stack_push(uplink_dll_t lks)
{
    if (top_cleanup_stack < CLEANUP_STACK_MAX - 1) {
        cleanup_stack[++top_cleanup_stack] = lks;
    }
    else {
        fprintf(stderr, "`cleanup_stack` full.\n");
        free(cleanup_stack);
        memory_free();
        exit(EXIT_FAILURE);
    }
}

uplink_dll_t cleanup_stack_pop()
{
    return cleanup_stack[top_cleanup_stack--];
}

bool cleanup_stack_trypop(uplink_dll_t *lks)
{
    if (top_cleanup_stack >= 0) {
        *lks = cleanup_stack[top_cleanup_stack--];
        return true;
    }
    return false;
}

/* ***** ***** */

#define FREE_STACK_MAX 65536

node_t *free_stack;

int32_t top_free_stack;

void free_stack_setup()
{
    free_stack = malloc(sizeof(node_t) * FREE_STACK_MAX);
    MALCHECKx(free_stack);
    top_free_stack = -1;
}

void free_stack_free()
{
    free(free_stack);
}

int free_stack_count()
{
    return top_free_stack + 1;
}

void free_stack_push(node_t x)
{
    if (top_free_stack < FREE_STACK_MAX - 1) {
        free_stack[++top_free_stack] = x;
    }
    else {
        fprintf(stderr, "`free_stack` full.\n");
        free(free_stack);
        memory_free();
        exit(EXIT_FAILURE);
    }
}

node_t free_stack_pop()
{
    return free_stack[top_free_stack--];
}

bool free_stack_trypop(node_t *x)
{
    if (top_free_stack >= 0) {
        *x = free_stack[top_free_stack--];
        return true;
    }
    return false;
}

/* ***** ***** */

void cleanup_uplink(uplink_t lk)
{
    node_t nd = get_node(lk);
    switch (get_rel(lk)) {
    case CHILD_REL: {
        leaf_t l = get_leaf(nd);
        cleanup_stack_push(get_single_parents(nd));
        cleanup_stack_push(get_leaf_parents(l));
        break;
    }
    default: {
        branch_t cc = get_cache(nd);
        if (cc != -1) {
            clear_cache(nd);
            add_to_parents(get_lchild_uplink(cc), get_lchild(nd));
            add_to_parents(get_rchild_uplink(cc), get_rchild(nd));
            cleanup_stack_push(get_branch_parents(nd));
        }
        break;
    }
    }
}

void cleanup()
{
    uplink_dll_t lks;
    while (cleanup_stack_trypop(&lks)) {
        iter_dll(&cleanup_uplink, lks);
    }
}

/* ***** ***** */

void lambda_scan(single_t s)
{
    cleanup_stack_push(get_leaf_parents(get_leaf(s)));
    node_t ch = get_child(s);
    while (get_node_kind(ch) == SINGLE_NODE) {
        s = ch;
        ch = get_child(s);
        cleanup_stack_push(get_leaf_parents(get_leaf(s)));
    }
    cleanup();
}

/* ***** ***** */

void clear_caches(single_t redlam, branch_t topapp)
{
    branch_t topcopy = get_cache(topapp);
    clear_cache(topapp);
    add_to_parents(get_lchild_uplink(topcopy), get_lchild(topcopy));
    add_to_parents(get_rchild_uplink(topcopy), get_rchild(topcopy));
    lambda_scan(redlam);
}

/* ***** ***** */

void del_par(node_t nd, uplink_t par)
{
    uplink_dll_t lks = get_parents(nd);
    if (is_lenght_one(lks)) {
        init_parents(nd);
        return;
    }
    uplink_t h = get_head(lks);
    if (h == par) set_head(lks, get_next(h));
    unlink(par);
}

/* ***** ***** */

void free_node(node_t nd)
{
    free_stack_push(nd);
    while (free_stack_trypop(&nd)) {
        switch (get_node_kind(nd)) {

        case LEAF_NODE:
            break;
        case SINGLE_NODE: {
            node_t ch = get_child(nd);
            del_par(ch, get_child_uplink(nd));
            if (is_empty(get_parents(ch))) free_stack_push(ch);
            dehalloc_single(nd);
            break;
        }
        case BRANCH_NODE: {
            node_t lch = get_lchild(nd);
            node_t rch = get_rchild(nd);
            del_par(lch, get_lchild_uplink(nd));
            del_par(rch, get_rchild_uplink(nd));
            if (is_empty(get_parents(lch))) free_stack_push(lch);
            if (is_empty(get_parents(rch))) free_stack_push(rch);
            dehalloc_branch(nd);
            break;
        }
        }
    }
}

/* ***** ***** */

// end of freenode.c