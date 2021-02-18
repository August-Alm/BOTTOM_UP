/* ***** ***** */

#ifndef VARNAMES_H
#define VARNAMES_H

/* ***** ***** */

#include "hmap.h"

/* ***** ***** */

void varnames_init();

bool varnames_lookup(char *key, struct var **val);

void varnames_add(char *key, struct var *val);

void varnames_clear();

void varnames_free();

/* ***** ***** */

#endif /* VARNAMES_H */
