/* ***** ***** */

#ifndef NAME_H
#define NAME_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

void setup_names();

struct name *get_name(char *str);

void decref_name(struct name *nam);

void clear_names();

void free_names();

/* ***** ***** */

#endif // NAME_H
