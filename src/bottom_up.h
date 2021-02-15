/* ***** ***** */

#ifndef BOTTOM_UP_H
#define BOTTOM_UP_H

/* ***** ***** */

//#include "types.h"
//#include "uplinks.h"
#include "dag.h"

/* ***** ***** */

struct var *new_var(char *name);

struct node new_lam(struct var *oldvar, struct node bod);

struct node new_app(struct node fun, struct node arg);

/* ***** ***** */

void replace_child(struct node newchild, struct uplink_dll *oldpars);

void clean_up(struct uplink_dll *uplink);

void clean_caches(struct lam *redlam, struct app *topapp);

void clear_dead_node(struct node node);

/* ***** ***** */

void upcopy(struct node newvar, struct uplink_dll *uplink);

void upcopy_uplinks(struct node new, struct uplink_dll *uplinks);

/* ***** ***** */

#endif // BOTTOM_UP_H
