/* ***** ***** */

#ifndef NORMALIZE_H
#define NORMALIZE_H

/* ***** ***** */

#include "types.h"
#include "upcopy.h"

/* ***** ***** */

struct node reduce(struct branch *b);

struct node normalize_wh(struct node nd);

struct node normalize(struct node nd);

/* ***** ***** */

#endif // NORMALIZE_H
