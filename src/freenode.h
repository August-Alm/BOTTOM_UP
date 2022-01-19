/* ***** ***** */

#ifndef FREENODE_H
#define FREENODE_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

void cleanup_stack_setup();

void cleanup_stack_free();

void free_stack_setup();

void free_stack_free();

/* ***** ***** */

void del_par(node_t nd, uplink_dll_t par);

void clear_caches(single_t redlam, branch_t topapp);

void free_node(node_t nd);

/* ***** ***** */

#endif // FREENODE_H