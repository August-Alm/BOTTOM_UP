/* ***** ***** */

#ifndef UPLINKS_H
#define UPLINKS_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

typedef void (*act_uplink) (struct uplink *uplk);

typedef void (*act2_uplink) (void* param, struct uplink *uplk);

/* ***** ***** */

struct uplink_list *get_uplinks(struct term t);

void add_uplink_to(struct term t, struct uplink *uplk);

void remove_uplink(struct uplink *uplk);

int is_length1(struct uplink_list uplinks);

void foreach_uplink(act_uplink f, struct uplink_list uplinks);

void foreach2_uplink(act2_uplink f, void* param, struct uplink_list uplinks);

/* ***** ***** */

#endif // UPLINKS_H

