/* ***** ***** */

#include "leaf.h"

/* ***** ***** */

extern
int32_t get_leaf_id(leaf_t l);

extern
int32_t get_leaf_name_id(leaf_t l);

char *get_leaf_name(leaf_t l)
{
    return get_name(get_leaf_name_id(l));
}

extern
void set_leaf_name_id(leaf_t l, int32_t nid);

extern
uplink_dll_t get_leaf_parents(leaf_t l);

extern
void set_leaf_parents(leaf_t l, uplink_dll_t lks);

/* ***** ***** */

// end of leaf.c