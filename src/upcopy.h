/* ***** ***** */

#ifndef UPCOPY_H
#define UPCOPY_H

/* ***** ***** */

#include <stdint.h>
#include <stdlib.h>
#include "malcheck.h"
#include "types.h"
#include "heap.h"
#include "uplink.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "node.h"
#include "freenode.h"

/* ***** ***** */

void upcopy_stack_setup();

void upcopy_stack_free();

void upcopy_stack_push(node_t newch, uplink_dll_t pars);

/* ***** ***** */

single_t new_single(leaf_t oldvar, node_t body);

branch_t new_branch(node_t func, node_t argm);

/* ***** ***** */

void upcopy();

/* ***** ***** */

#endif // UPCOPY_H