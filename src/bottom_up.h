/* ***** ***** */

#ifndef BOTTOM_UP_H
#define BOTTOM_UP_H

/* ***** ***** */

#include "context.h"
#include "dag.h"

/* ***** ***** */

struct var *new_var(char *name);

struct term new_lam(struct var *oldvar, struct term bod);

struct term new_app(struct term fun, struct term arg);

/* ***** ***** */

void replace_child(struct term newchild, struct uplink_list oldpars);

void clean_up(struct uplink *uplk);

void clean_caches(struct lam *redlam, struct app *topapp);

void clear_dead_term(struct term t);

/* ***** ***** */

void upcopy(struct term newt, struct uplink *uplk);

void upcopy_uplinks(struct term newt, struct uplink_list uplinks);

/* ***** ***** */

#endif // BOTTOM_UP_H
