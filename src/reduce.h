/* ***** ***** */

#ifndef REDUCE_H
#define REDUCE_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

void single_stack_setup();

void single_stack_free();

/* ***** ***** */

node_t reduce(branch_t redex);

/* ***** ***** */

#endif // REDUCE_H