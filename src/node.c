/* ***** ***** */

#include "node.h"

/* ***** ***** */

extern
enum node_kind get_node_kind(node_t nd);

extern
uplink_dll_t get_parents(node_t nd);

extern
void set_parents(node_t nd, uplink_dll_t lks);

extern
void init_parents(node_t nd);

extern
void add_to_parents(uplink_t lk, node_t nd);

/* ***** ***** */

// end of node.c