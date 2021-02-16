/* ***** ***** */

#include <stdlib.h>
#include "bottom_up.h"
#include "basics.h"

/* ***** ***** */

struct var *new_var(char *name)
{
    struct var *x = var_halloc();
    if (!x) {
        free(name);
        memory_free();
    }
    store_name(name);
    x->name = name;
    return x;
}

struct node new_lam(struct var *oldvar, struct node bod)
{
    struct var *newvar = new_var(oldvar->name);
    struct lam *l = lam_halloc();
    if (!l) { memory_free(); }
    l->var = newvar;
    l->bod = bod;
    add_uplink_to(bod, &(l->bod_uplink));
    struct node oldnd = (struct node) { .term = oldvar };
    upcopy_uplinks(oldnd, oldvar->uplinks);
    return (struct node) { .term = l };
}

struct node new_app(struct node fun, struct node arg)
{
    struct app *a = app_halloc();
    if (!a) { memory_free(); }
    a->fun = fun;
    a->arg = arg;
    return (struct node) { .term = a };
}

/* ***** ***** */

void clean_up(struct uplink_dll *uplink)
{
    int kind = uplink->kind;

    if (kind == BOD_UPLINK) {
        struct lam *l = LAM_OF_BOD(uplink);
        foreach_uplink(clean_up, l->uplinks);
        foreach_uplink(clean_up, l->var->uplinks);
        l->uplinks = NULL;
        l->var->uplinks = NULL;
        return;
    }
    struct app *cpy;
    if (kind == FUN_UPLINK) {
        cpy = APP_OF_FUN(uplink)->cache;
    }
    else {
        cpy = APP_OF_ARG(uplink)->cache;
    }
    if (cpy) {
        struct node fun = cpy->fun;
        struct node arg = cpy->arg;
        struct uplink_dll *uplinks = cpy->uplinks;
        add_uplink_to(fun, &(cpy->fun_uplink));
        add_uplink_to(arg, &(cpy->arg_uplink));
        cpy = NULL;
        foreach_uplink(clean_up, uplinks);
        uplinks = NULL;
    } 
}

void lambda_scan(struct lam *l)
{
    struct node bod = l->bod;
    if (bod.term) {
        foreach_uplink(clean_up, l->var->uplinks);
        l->var->uplinks = NULL;
        if (KIND(bod) == LAM_NODE) {
            lambda_scan(LAM(bod));
        }
    }
}

void clean_caches(struct lam *redlam, struct app *topapp)
{
    struct node fun = topapp->fun;
    struct node arg = topapp->arg;
    
    add_uplink_to(fun, &(topapp->fun_uplink));
    add_uplink_to(arg, &(topapp->arg_uplink));
    
    topapp->cache = NULL;

    lambda_scan(redlam);
}

/* ***** ***** */

static inline
void install_child(struct node child, struct uplink_dll *parent)
{
    switch (parent->kind) {
    case BOD_UPLINK:
        LAM_OF_BOD(parent)->bod = child;
        break;
    case FUN_UPLINK:
        APP_OF_FUN(parent)->fun = child;
        break;
    case ARG_UPLINK:
        APP_OF_ARG(parent)->arg = child;
        break;
    }
}

void replace_child(struct node newchild, struct uplink_dll *oldpars)
{
    if (!oldpars) { return; }
    
    struct uplink_dll *tmp = oldpars;
    struct uplink_dll *tmpnxt = tmp->next;
    while (tmpnxt) {
        install_child(newchild, tmp);
        tmp = tmpnxt;
        tmpnxt = tmp -> next;
    }
    install_child(newchild, tmp);
    
    switch (KIND(newchild)) {
    case VAR_NODE:
        tmpnxt = VAR(newchild)->uplinks;
        VAR(newchild)->uplinks = oldpars;
    case LAM_NODE:
        tmpnxt = LAM(newchild)->uplinks;
        LAM(newchild)->uplinks = oldpars;
    case APP_NODE:
        tmpnxt = APP(newchild)->uplinks;
        APP(newchild)->uplinks = oldpars;
    }
    
    oldpars = NULL;
}

/* ***** ***** */

static inline
void delete_up(struct node nd, struct uplink_dll uplink)
{
    struct uplink_dll *uplinks = get_uplinks(nd);
    if (uplinks) {
        remove_uplink(uplink);
        if (!uplinks) { clear_dead_node(nd); }
    }
}

void clear_node(struct node nd)
{
    switch (KIND(nd)) {
    case VAR_NODE:
        var_clear(VAR(nd));
        break;
    case LAM_NODE:
        if (LAM(nd)->bod.term) {
            delete_up(LAM(nd)->bod, LAM(nd)->bod_uplink);
        }
        lam_clear(LAM(nd));
        break;
    case APP_NODE:
        if (APP(nd)->fun.term) {
            delete_up(APP(nd)->fun, APP(nd)->fun_uplink);
        }
        if (APP(nd)->arg.term) {
            delete_up(APP(nd)->arg, APP(nd)->arg_uplink);
        }
        app_clear(APP(nd));
        break;
    }
}

void clear_dead_node(struct node dead)
{
    clear_node(dead);
}

/* ***** ***** */

void upcopy(struct node newchild, struct uplink_dll *uplink)
{
    struct lam *l;
    struct app *a;
    struct node new;

    switch (uplink->kind) {
    case BOD_UPLINK:
        l = LAM_OF_BOD(uplink);
        new = new_lam(l->var, newchild);
        upcopy_uplinks(new, l->uplinks);
        break;
    case FUN_UPLINK:
        a = APP_OF_FUN(uplink);
        if (a->cache) {
            a->cache->fun = newchild;
        }
        else {
            new = new_app(newchild, a->arg);
            a->cache = APP(new);
            upcopy_uplinks(new, a->uplinks);
        }
        break;
    case ARG_UPLINK:
        a = APP_OF_ARG(uplink);
        if (a->cache) {
            a->cache->arg = newchild;
        }
        else {
            new = new_app(a->fun, newchild);
            a->cache = APP(new);
            upcopy_uplinks(new, a->uplinks);
        }
        break;
    }
}

inline
void upcopy_uplinks(struct node new, struct uplink_dll *uplinks)
{
    struct uplink_dll *tmp;
    while (tmp) {
        upcopy(new, tmp);
        tmp = tmp->next;
    }
}


