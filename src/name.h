/* ***** ***** */

#ifndef NAME_H
#define NAME_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

void names_setup();

void names_free();

/* ***** ***** */

int32_t get_name_id(char *str);

char *get_name(int32_t nid);

int32_t add_name(char *str);

/* ***** ***** */

#endif // NAME_H