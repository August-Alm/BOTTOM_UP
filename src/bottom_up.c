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
    x->name = name;
    varnames_add(name, x);
    return x;
}

struct term new_lam(struct var *oldvar, struct term bod)
{
    struct var *newvar = new_var(oldvar->name);
    struct lam *l = lam_halloc();
    if (!l) { memory_free(); }
    l->var = newvar;
    l->bod = bod;
    add_uplink_to(bod, &(l->bod_uplink));
    struct term oldt = (struct term) { .ptr = oldvar };
    upcopy_uplinks(oldt, oldvar->uplinks);
    return (struct term) { .ptr = l };
}

struct term new_app(struct term fun, struct term arg)
{
    struct app *a = app_halloc();
    if (!a) { memory_free(); }
    a->fun = fun;
    a->arg = arg;
    return (struct term) { .ptr = a };
}

/* ***** ***** */

void clean_up(struct uplink *uplink)
{
    int kind = uplink->kind;

    if (kind == BOD_UPLINK) {
        struct lam *l = LAM_OF_BOD(uplink);
        foreach_uplink(clean_up, l->uplinks);
        foreach_uplink(clean_up, l->var->uplinks);
        l->uplinks.head = NULL;
        l->var->uplinks.head = NULL;
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
        struct term fun = cpy->fun;
        struct term arg = cpy->arg;
        struct uplink_list uplinks = cpy->uplinks;
        add_uplink_to(fun, &(cpy->fun_uplink));
        add_uplink_to(arg, &(cpy->arg_uplink));
        cpy = NULL;
        foreach_uplink(clean_up, uplinks);
        uplinks.head = NULL;
    } 
}

void lambda_scan(struct lam *l)
{
    struct term bod = l->bod;
    if (bod.ptr) {
        foreach_uplink(clean_up, l->var->uplinks);
        l->var->uplinks.head = NULL;
        if (KIND(bod) == LAM_TERM) {
            lambda_scan(LAM(bod));
        }
    }
}

void clean_caches(struct lam *redlam, struct app *topapp)
{
    struct term fun = topapp->fun;
    struct term arg = topapp->arg;
    
    add_uplink_to(fun, &(topapp->fun_uplink));
    add_uplink_to(arg, &(topapp->arg_uplink));
    
    topapp->cache = NULL;

    lambda_scan(redlam);
}

/* ***** ***** */

static inline
void install_child(struct term child, struct uplink *parent)
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

void replace_child(struct term newchild, struct uplink_list oldpars)
{
    if (!oldpars.head) { return; }
    
    struct uplink *tmp = oldpars.head;
    struct uplink *tmpnxt = tmp->next;
    while (tmpnxt) {
        install_child(newchild, tmp);
        tmp = tmpnxt;
        tmpnxt = tmp -> next;
    }
    install_child(newchild, tmp);
    
    switch (KIND(newchild)) {
    case VAR_TERM:
        tmpnxt = VAR(newchild)->uplinks.head;
        VAR(newchild)->uplinks = oldpars;
        break;
    case LAM_TERM:
        tmpnxt = LAM(newchild)->uplinks.head;
        LAM(newchild)->uplinks = oldpars;
        break;
    case APP_TERM:
        tmpnxt = APP(newchild)->uplinks.head;
        APP(newchild)->uplinks = oldpars;
    }
    
    oldpars.head = NULL;
}

/* ***** ***** */

static inline
void delete_up(struct term t, struct uplink *uplk)
{
    struct uplink_list *uplinks = get_uplinks(t);
    if (uplinks->head) {
        remove_uplink(uplk);
        if (!uplinks->head) { clear_dead_term(t); }
    }
}

void clear_term(struct term t)
{
    switch (KIND(t)) {
    case VAR_TERM:
        var_clear(VAR(t));
        break;
    case LAM_TERM:
        if (LAM(t)->bod.ptr) {
            delete_up(LAM(t)->bod, &(LAM(t)->bod_uplink));
        }
        lam_clear(LAM(t));
        break;
    case APP_TERM:
        if (APP(t)->fun.ptr) {
            delete_up(APP(t)->fun, &(APP(t)->fun_uplink));
        }
        if (APP(t)->arg.ptr) {
            delete_up(APP(t)->arg, &(APP(t)->arg_uplink));
        }
        app_clear(APP(t));
        break;
    }
}

void clear_dead_term(struct term dead)
{
    clear_term(dead);
}

/* ***** ***** */

void upcopy(struct term newchild, struct uplink *uplk)
{
    struct lam *l;
    struct app *a;
    struct term newt;

    switch (uplk->kind) {
    case BOD_UPLINK:
        l = LAM_OF_BOD(uplk);
        newt = new_lam(l->var, newchild);
        upcopy_uplinks(newt, l->uplinks);
        break;
    case FUN_UPLINK:
        a = APP_OF_FUN(uplk);
        if (a->cache) {
            a->cache->fun = newchild;
        }
        else {
            newt = new_app(newchild, a->arg);
            a->cache = APP(newt);
            upcopy_uplinks(newt, a->uplinks);
        }
        break;
    case ARG_UPLINK:
        a = APP_OF_ARG(uplk);
        if (a->cache) {
            a->cache->arg = newchild;
        }
        else {
            newt = new_app(a->fun, newchild);
            a->cache = APP(newt);
            upcopy_uplinks(newt, a->uplinks);
        }
        break;
    }
}

inline
void upcopy_uplinks(struct term newt, struct uplink_list uplinks)
{
    struct uplink *tmp = uplinks.head;
    while (tmp) {
        upcopy(newt, tmp);
        tmp = tmp->next;
    }
}
