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
struct term {
    void* ptr;
};

#define TERM(p) ((struct term) { .ptr = p })

#define VAR_TERM 1
#define LAM_TERM 2
#define APP_TERM 3

#define KIND(term)                                     \
    (*(int*)(term.ptr) == -1 ? VAR_TERM :              \
        *(int*)(term.ptr) == 1 ? LAM_TERM : APP_TERM)  \

#define VAR(term) ((struct var*)term.ptr)
#define LAM(term) ((struct lam*)term.ptr)
#define APP(term) ((struct app*)term.ptr)
 
/* ***** ***** */

/* This struct morally also holds either a `struct lam` or a `struct
 * app`. However, these `struct uplink` are always allocated inside a
 * `struct lam` or `struct app`, which we can thus retreive by pointer
 * arithmetic.
 */
struct uplink {
    struct uplink *next;
    struct uplink *prev;
    int kind; // Always `BOD_`, `FUN_` or `ARG_UPLINK`.
};

struct uplink_list {
    struct uplink *head;
};

#define BOD_UPLINK 1
#define FUN_UPLINK 2
#define ARG_UPLINK 3

// `__builtin_offset` is gcc specific.
#define CONTAINER_OF(ptr, type, field) \
    ((type*)((char*)(1 ? (ptr) : &((type*)0)->field) - __builtin_offsetof(type, field)))
 
#define LAM_OF_BOD(uplk) CONTAINER_OF(uplk, struct lam, bod_uplink)
#define APP_OF_FUN(uplk) CONTAINER_OF(uplk, struct app, fun_uplink)
#define APP_OF_ARG(uplk) CONTAINER_OF(uplk, struct app, arg_uplink)

/* ***** ***** */

struct var {
    int minus_one; // Always set to -1.
    char *name;
    struct uplink_list uplinks;
};

/* ***** ***** */

struct lam {
    struct var *var;
    struct term bod;
    struct uplink bod_uplink;
    struct uplink_list uplinks;
};

/* ***** ***** */

struct app {
    struct term fun;
    struct term arg;
    struct uplink fun_uplink;
    struct uplink arg_uplink;
    struct uplink_list uplinks;
    struct app* cache; 
};

/* ***** ***** */

#endif // TYPES_H
