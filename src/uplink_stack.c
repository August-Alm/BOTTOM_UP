/* ***** ***** */

#include <stdlib.h>
#include "uplink_stack.h"

/* ***** ***** */

struct uplink *pop(struct uplink_stack *stk)
{
    struct uplink *top = stk->top;
    struct uplink *nxt = next_uplink(top);
    if (nxt) {
        stk->top = nxt;
    }
    else {
        struct uplink_stack *tmp = stk;
        *stk = *stk->pending;
        free(tmp);
    }
    return top;
}

bool push_many(struct uplink_dll lks, struct uplink_stack **stk)
{
    struct uplink_stack *new = malloc(sizeof(struct uplink_stack));
    if (!new) { return false; }
    struct uplink *h = head_of(lks);
    if (!h) { return true; }
    *new = (struct uplink_stack) {
        .top = h,
        .pending = *stk
    };
    *stk = new;
    return true;
}

/* ***** ***** */

// end of uplink_stack.c
