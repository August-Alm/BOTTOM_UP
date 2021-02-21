/* ***** ***** */

#include <stddef.h>
#include "reduction.h"
#include<stdio.h>

/* ***** ***** */

#define POP(data, stack) do { \
    data = (*stack)->current; \
    *stack = (*stack)->next;  \
} while(0)

#define PUSH(data, stack) do { \
    *stack = &(struct sll) {   \
        .next = *stack,        \
        .current = data        \
    };                         \
} while(0)

/* ***** ***** */

static inline
void scan_down(struct term t, struct app *app,
               struct term *l, struct app **a)
{
    switch (KIND(t)) {
    case VAR_TERM:
        *l = app->arg;
        *a = NULL;
        break;
    case LAM_TERM:
        scan_down(LAM(t)->bod, app, l, a);
        *l = new_lam(LAM(t)->var, *l);
        break;
    case APP_TERM:
        *l = new_app(APP(t)->fun, APP(t)->arg);
        APP(t)->cache = APP((*l));
        upcopy_uplinks(app->arg, LAM(app->fun)->var->uplinks);
        *a = APP(t);
        break;
    }
}

struct term reduce(struct app *app)
{
    if (KIND(app->fun) != LAM_TERM) {
        printf("This is irreducible.\n");
        return (struct term) { .ptr = NULL };
    }
    struct lam *funlam = LAM(app->fun);
    struct uplink_list funuplks = funlam->uplinks;
    struct uplink_list varuplks = funlam->var->uplinks;
    
    struct term ans;
    
    if (is_length1(funuplks)) {
        replace_child(app->arg, funuplks);
        ans = funlam->bod;
    }
    else if (!varuplks.head) {
        ans = funlam->bod;
    }

    struct app *topapp = app_halloc();
    scan_down(funlam->bod, app, &ans, &topapp);
    if (topapp) {
        clean_caches(funlam, topapp);
    }
    replace_child(ans, app->uplinks);
    clear_dead_term((struct term) { .ptr = app });
    fprintf_term(stdout, ans);
    return ans;
}

void normalize_wh(struct term t)
{
    if (!t.ptr) { return; }
    int kind = KIND(t);
    if (kind == APP_TERM) {
        normalize_wh(APP(t)->fun);
        if (KIND(APP(t)->fun) == LAM_TERM) {
            normalize_wh(reduce(APP(t)));
        }
    }
}

//void normalize_wh(struct node node)
//{
//    struct sll { struct sll *next; struct node current; };
//    struct sll init = { NULL, node };
//    struct sll *pinit = &init;
//    struct sll **stack = &pinit;
//
//    struct node nd;
//
//    while (*stack) {
//        POP(nd, stack);
//        int kind = KIND(nd);
//        if (kind != APP_NODE) {
//            continue;
//        }
//        struct app *app = APP(nd);
//        struct node fun = app->fun;
//        while (KIND(fun) == APP_NODE && (APP(fun)->fun).term) {
//            app = APP(fun);
//            fun = app->fun;
//        }
//        if (kind == LAM_NODE) {
//            PUSH(reduce(app), stack);
//        }
//    }
//}

void normalize(struct term t)
{
    if (!t.ptr) {
        printf("Should not happen.\n");
        return;
    }
    
    switch (KIND(t)) {

    case VAR_TERM:
        break;
    
    case LAM_TERM:
        normalize(LAM(t)->bod);
        break;

    case APP_TERM:
        normalize_wh(APP(t)->fun);
        
        switch (KIND(APP(t)->fun)) {
        case VAR_TERM:
            normalize(APP(t)->arg);
            break;
        case LAM_TERM:
            normalize(reduce(APP(t)));
            break;
        case APP_TERM:
            normalize(APP(t)->fun);
            normalize(APP(t)->arg);
            break;
        }
    }
}
