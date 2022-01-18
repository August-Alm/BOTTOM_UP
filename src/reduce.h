/* ***** ***** */

#ifndef REDUCE_H
#define REDUCE_H

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
#include "upcopy.h"

/* ***** ***** */

void single_stack_setup();

void single_stack_free();

/* ***** ***** */

node_t reduce(branch_t redex);

/* ***** ***** */

#endif // REDUCE_H
