#ifndef UPLINK_STACK_H
#define ULINK_STACK_H

/* ***** ***** */

#include <stdbool.h>
#include "types.h"
#include "uplink.h"

/* ***** ***** */

struct uplink_stack {
    struct uplink *top;
    struct uplink_stack *pending;
};

/* ***** ***** */

struct uplink *pop(struct uplink_stack *stk);

bool push_many(struct uplink_dll lks, struct uplink_stack **stk);

/* ***** ***** */

#endif // UPLINK_STACK_H
