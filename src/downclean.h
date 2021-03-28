/* ***** ***** */

#ifndef DOWNCLEAN_H
#define DOWNCLEAN_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

void replace_child(struct node nd, struct uplink_dll *lks);

void downclean(struct node contractum, struct branch *redex);

void downclean_is_length_one(struct node contractum, struct branch *redex);

/* ***** ***** */

#endif // DOWNCLEAN_H
