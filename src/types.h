/* ***** ***** */

#ifndef TYPES_H
#define TYPES_H

/* ***** ***** */

#include <stdint.h>

/* ***** ***** */

typedef uint32_t address_t;

/* ***** ***** */

enum node_kind { LEAF_NODE, SINGLE_NODE, BRANCH_NODE };

struct node {
    address_t address;
};

/* ***** ***** */

enum uplink_rel { CHILD_REL, LCHILD_REL, RCHILD_REL };

struct uplink {
    address_t next;
    address_t prev;
    enum uplink_rel rel;
};

struct uplink_dll {
    address_t head;
};

/* ***** ***** */

struct name {
    char *str;
    uint32_t refcnt;
};

struct leaf {
    uint32_t id;    
    struct uplink_dll parents;
    struct name *name;
};

/* ***** ***** */

struct single {
    uint32_t id;
    address_t leaf;
    struct node child;
    struct uplink_dll parents;
    struct uplink child_uplink;
};

/* ***** ***** */

struct branch {
    uint32_t id;
    struct node lchild;
    struct node rchild;
    struct uplink_dll parents;
    struct node cache;
    struct uplink lchild_uplink;
    struct uplink rchild_uplink;
};

/* ***** ***** */

#endif // TYPES_H
