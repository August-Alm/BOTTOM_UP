/* ***** ***** */

#ifndef UPCOPY_H
#define UPCOPY_H

/* ***** ***** */

#include <stdbool.h>
#include "types.h"
#include "uplink_stack.h"

/* ***** ***** */

struct upcopy_state {
    struct uplink_stack *stack;
    struct node ccache;
    struct node pcache;
};

/* ***** ***** */

struct upcopy_state *init_upcopy_state(struct branch *redex);

bool step_upcopy_state(struct upcopy_state *state);

/* ***** ***** */

#endif // UPCOPY_H
