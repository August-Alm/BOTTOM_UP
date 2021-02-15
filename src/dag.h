/* ***** ***** */

#ifndef DAG_H
#define DAG_H

/* ***** ***** */

#include "types.h"
#include "uplinks.h"

/* ***** ***** */

void heap_setup();

void memory_clear();

void memory_free();

/* ***** ***** */

void store_name(char *name);

/* ***** ***** */

struct var *var_halloc();

void var_clear(struct var *var);

struct lam *lam_halloc();

void lam_clear(struct lam *lam);

struct app *app_halloc();

void app_clear(struct app *app);

/* ***** ***** */

#endif // DAG_H
