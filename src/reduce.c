/* ***** ***** */

#include "memory.h"
#include "reduce.h"
#include "malcheck.h"
#include "heap.h"
#include "uplink.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "node.h"
#include "freenode.h"
#include "upcopy.h"
#include <stdint.h>
#include <stdlib.h>

/* ***** ***** */

#define SINGLE_STACK_MAX 65536

single_t *single_stack;

int32_t top_single_stack;

void single_stack_setup()
{
    single_stack = malloc(sizeof(single_t) * SINGLE_STACK_MAX);
    MALCHECKx(single_stack);
    top_single_stack = -1;
}

void single_stack_free()
{
    free(single_stack);
}

static inline
int single_stack_count()
{
    return top_single_stack + 1;
}

static inline
void single_stack_push(single_t x)
{
    if (top_single_stack < SINGLE_STACK_MAX - 1) {
        single_stack[++top_single_stack] = x;
    }
    else {
        fprintf(stderr, "`single_stack` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
}

static inline
single_t single_stack_pop()
{
    return single_stack[top_single_stack--];
}

static inline
bool single_stack_trypop(single_t *x)
{
    if (top_single_stack >= 0) {
        *x = single_stack[top_single_stack--];
        return true;
    }
    return false;
}

/* ***** ***** */

struct nodebranch {
    node_t deep_child;
    branch_t top_app;
};

static
struct nodebranch helper
    (node_t nd, enum node_kind knd, node_t argm, uplink_dll_t varpars)
{
    if (knd == LEAF_NODE) {
        return (struct nodebranch) {
            .deep_child = argm, .top_app = -1
        };
    }
    branch_t cc = new_branch(get_lchild(nd), get_rchild(nd));
    set_cache(nd, cc);
    upcopy_stack_push(argm, varpars);
    return (struct nodebranch) {
        .deep_child = cc, .top_app = nd
    };
}

static
struct nodebranch scandown(node_t nd, node_t argm, uplink_dll_t varpars)
{
    struct nodebranch dch_topapp;
    enum node_kind knd = get_node_kind(nd);
    if (knd == SINGLE_NODE) {
        single_t s = nd;
        node_t ch = get_child(s);        
        knd = get_node_kind(ch);
        single_stack_push(s);
        while (knd == SINGLE_NODE) {
            s = ch;
            ch = get_child(s);
            knd = get_node_kind(ch);
            single_stack_push(s);
        }
        dch_topapp = helper(ch, knd, argm, varpars);
    }
    else {
        dch_topapp = helper(nd, knd, argm, varpars);
    }
    node_t dch = dch_topapp.deep_child;
    single_t g;
    while (single_stack_count() > 0) {
        g = single_stack_pop();
        dch = new_single(get_leaf(g), dch);
    }
    dch_topapp.deep_child = dch;
    return dch_topapp;
}

/* ***** ***** */

static
void install_child(node_t nd, uplink_t lk)
{
    switch (get_rel(lk)) {
    case CHILD_REL:
        set_child(get_node(lk), nd);
        break;
    case LCHILD_REL:
        set_lchild(get_node(lk), nd);
        break;
    case RCHILD_REL:
        set_rchild(get_node(lk), nd);
        break;
    }
}

static
void replace_child(node_t newch, uplink_dll_t oldpars)
{
    if (is_empty(oldpars)) return;
    uplink_t lk = get_head(oldpars);
    uplink_t nxt = get_next(lk);
    install_child(newch, lk);
    while (nxt != -1) {
        lk = nxt;
        nxt = get_next(lk);
        install_child(newch, lk);
    }
    uplink_dll_t newpars = get_parents(newch);
    if (!is_empty(newpars)) link(lk, get_head(newpars));
    set_head(newpars, get_head(oldpars));
    init_dll(oldpars);
}

/* ***** ***** */

node_t reduce(branch_t redex)
{
    single_t func = get_lchild(redex);
    node_t argm = get_rchild(redex);
    leaf_t var = get_leaf(func);
    node_t body = get_child(func);
    uplink_dll_t lampars = get_single_parents(func);
    uplink_dll_t varpars = get_leaf_parents(var);

    if (is_empty(varpars)) {
        replace_child(body, get_branch_parents(redex));
        free_node(redex);
        return body;
    }
    
    if (is_lenght_one(lampars)) {
        replace_child(argm, varpars);
        node_t answer = get_child(func);
        replace_child(answer, get_branch_parents(redex));
        del_par(argm, get_rchild_uplink(redex));
        del_par(func, get_lchild_uplink(redex));
        dehalloc_branch(redex);
        del_par(get_child(func), get_child_uplink(func));
        dehalloc_single(func);
        return answer;
    }

    struct nodebranch dch_topapp = scandown(body, argm, varpars);
    node_t answer = dch_topapp.deep_child;
    node_t topapp = dch_topapp.top_app;

    upcopy();

    if (topapp != -1) clear_caches(func, topapp);
    replace_child(answer, get_branch_parents(redex));
    free_node(redex);
    return answer;
}

/* ***** ***** */

// end of reduce.c