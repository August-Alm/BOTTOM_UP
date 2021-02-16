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
void scan_down(struct node nd, struct app *app,
               struct node l, struct app *a)
{
    switch (KIND(nd)) {
    case VAR_NODE:
        l = app->arg;
        a = NULL;
        break;
    case LAM_NODE:
        scan_down(LAM(nd)->bod, app, l, a);
        l = new_lam(LAM(nd)->var, l);
        break;
    case APP_NODE:
        l = new_app(APP(nd)->fun, APP(nd)->arg);
        APP(nd)->cache = APP(l);
        upcopy_uplinks(app->arg, LAM(app->fun)->var->uplinks);
        a = APP(nd);
        break;
    }
}

struct node reduce(struct app *app)
{
    if (KIND(app->fun) != LAM_NODE) {
        printf("This is irreducible.\n");
        return (struct node) { .term = NULL };
    }
    struct lam *funlam = LAM(app->fun);
    struct uplink_dll *funuplks = funlam->uplinks;
    struct uplink_dll *varuplks = funlam->var->uplinks;
    
    struct node ans;
    
    if (is_length1(funuplks)) {
        replace_child(app->arg, funuplks);
        ans = funlam->bod;
    }
    else if (!varuplks) {
        ans = funlam->bod;
    }

    struct app *topapp = app_halloc();
    scan_down(funlam->bod, app, ans, topapp);
    if (topapp) {
        clean_caches(funlam, topapp);
    }
    replace_child(ans, app->uplinks);
    clear_dead_node((struct node) { .term = app });
    return ans;
}

void normalize_wh(struct node nd)
{
    if (!nd.term) { return; }
    int kind = KIND(nd);
    if (kind == APP_NODE) {
        normalize_wh(APP(nd)->fun);
        if (KIND(APP(nd)->fun) == LAM_NODE) {
            normalize_wh(reduce(APP(nd)));
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

void normalize(struct node nd)
{
    if (!nd.term) {
        printf("Should not happen.\n");
        return;
    }
    
    switch (KIND(nd)) {

    case VAR_NODE:
        break;
    
    case LAM_NODE:
        normalize(LAM(nd)->bod);
        break;

    case APP_NODE:
        normalize_wh(APP(nd)->fun);
        
        switch (KIND(APP(nd)->fun)) {
        case VAR_NODE:
            normalize(APP(nd)->arg);
            break;
        case LAM_NODE:
            normalize(reduce(APP(nd)));
            break;
        case APP_NODE:
            normalize(APP(nd)->fun);
            normalize(APP(nd)->arg);
            break;
        }
    }
}
