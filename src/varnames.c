/* ***** ***** */

#include <stddef.h>
#include "varnames.h"

/* ***** ***** */

struct hmap *varnames = NULL;

void varnames_init()
{
    varnames = hmap_create();
}

bool varnames_lookup(char *key, struct var **val)
{
    return hmap_lookup(varnames, key, val);
}

void varnames_add(char *key, struct var *val)
{
    hmap_add(varnames, key, val);
}

void varnames_clear()
{
    hmap_clear(varnames);
}

void varnames_free()
{
    hmap_free(varnames);
}
