/* ***** ***** */

#ifndef PARSER_H
#define PARSER_H

/* ***** ***** */

#include "bottom_up.h"

/* ***** ***** */

void fprintf_term(FILE *out, struct term t);

struct term parse_term(FILE *inp);

/* ***** ***** */

#endif // PARSER_H


