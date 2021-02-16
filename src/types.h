/* ***** ***** */

#ifndef TYPES_H
#define TYPES_H

/* ***** ***** */

struct var;
struct lam;
struct app;

/* We use a memory layout such that we can avoid making this
 * the obvious tagged union. See `KIND` macro, below.
 */
struct node {
    void* term;
};

#define VAR_NODE 1
#define LAM_NODE 2
#define APP_NODE 3

#define KIND(node)                                      \
    (*(int*)(node.term) == -1 ? VAR_NODE :              \
        *(int*)(node.term) == 1 ? LAM_NODE : APP_NODE)  \

#define VAR(node) ((struct var*)node.term)
#define LAM(node) ((struct lam*)node.term)
#define APP(node) ((struct app*)node.term)

/* ***** ***** */

/* This struct morally also holds either a `struct lam` or `struct
 * app`. However, these `struct uplink_dll` are always allocated
 * inside a `struct lam` or `struct app`, which we can thus
 * retreive by pointer arithmetic. See`get_node` in `uplinks.h/c`.
 */
struct uplink_dll {
    int kind; // Always `BOD`, `FUN` or `ARG`.
    struct uplink_dll *next;
    struct uplink_dll *prev;
};

#define BOD_UPLINK 1
#define FUN_UPLINK 2
#define ARG_UPLINK 3

/* ***** ***** */

struct var {
    int minus_one; // Always set to -1.
    char *name;
    struct uplink_dll *uplinks;
};

/* ***** ***** */

struct lam {
    struct uplink_dll bod_uplink;
    struct uplink_dll *uplinks;
    struct var *var;
    struct node bod;
};

/* ***** ***** */

struct app {
    struct uplink_dll fun_uplink;
    struct uplink_dll arg_uplink;
    struct uplink_dll *uplinks;
    struct node fun;
    struct node arg;
    struct app* cache; 
};

/* ***** ***** */

#endif // TYPES_H
