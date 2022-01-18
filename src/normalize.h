/* ***** ***** */

#ifndef NORMALIZE_H
#define NORMALIZE_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

void norm_stack_setup();

void norm_stack_free();

/* ***** ***** */

void normalize_wh(node_t *nd);

void normalize(node_t *nd);

/* ***** ***** */

#endif // NORMALIZE_H