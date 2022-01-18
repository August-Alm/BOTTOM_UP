/* ***** ***** */

#ifndef TYPES_H
#define TYPES_H

/* ***** ***** */

#include <stdint.h>

/* ***** ***** */

typedef int32_t address_t;

/* ***** ***** */

enum node_kind {
    LEAF_NODE,
    SINGLE_NODE,
    BRANCH_NODE
};

typedef int32_t node_t;

/* ***** ***** */

enum uplink_rel {
    CHILD_REL,
    LCHILD_REL,
    RCHILD_REL
};

typedef int32_t uplink_t;

struct uplink {
    int32_t rel;
    int32_t next;
    int32_t prev;
};

/* ***** ***** */

typedef int32_t uplink_dll_t;

/* ***** ***** */

struct name {
    char *str;
    uint32_t refcnt;
};

/* ***** ***** */

typedef int32_t leaf_t;

struct leaf {
    int32_t id;
    uplink_dll_t parents;
};

/* ***** ***** */

typedef int32_t single_t;

struct single {
    int32_t id;
    struct leaf leaf;
    node_t child;
    struct uplink child_uplink;
    uplink_dll_t parents;
};

/* ***** ***** */

typedef int32_t branch_t;

struct branch {
    int32_t id;
    node_t lchild;
    node_t rchild;
    struct uplink lchild_uplink;
    struct uplink rchild_uplink;
    uplink_dll_t parents;
    branch_t cache;
};

/* ***** ***** */

#endif // TYPES_H
