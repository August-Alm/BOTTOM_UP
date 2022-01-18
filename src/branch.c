/* ***** ***** */

#include "branch.h"

/* ***** ***** */

extern
node_t get_lchild(branch_t b);

extern
void set_lchild(branch_t b, node_t lch);

extern
node_t get_rchild(branch_t b);

extern
void set_rchild(branch_t b, node_t rch);

extern
uplink_t get_lchild_uplink(branch_t b);

extern
uplink_t get_rchild_uplink(branch_t b);

extern
uplink_dll_t get_branch_parents(branch_t b);

extern
void set_branch_parents(branch_t b, uplink_dll_t lks);

extern
branch_t get_cache(branch_t b);

extern
void set_cache(branch_t b, branch_t cc);

extern
void clear_cache(branch_t b);

/* ***** ***** */

// end of branch.c