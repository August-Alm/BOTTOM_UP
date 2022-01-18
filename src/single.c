/* ***** ***** */

#include "single.h"

/* ***** ***** */

extern
int32_t get_single_id(single_t s);

extern
leaf_t get_leaf(single_t s);

extern
node_t get_child(single_t s);

extern
void set_child(single_t s, node_t ch);

extern
uplink_t get_child_uplink(single_t s);

extern
uplink_dll_t get_single_parents(single_t s);

extern
void set_single_parents(single_t s, uplink_dll_t lks);

extern
void init_single(single_t s, int32_t nid);

/* ***** ***** */

// end of single.c