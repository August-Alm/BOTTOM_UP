/* ***** ***** */

#ifndef PARSER_H
#define PARSER_H

/* ***** ***** */

#include "bottom_up.h"

/* ***** ***** */

void fprintf_node(FILE *out, struct node nd);

struct node parse_node(FILE *inp, struct hmap *h);

/* ***** ***** */

#endif // PARSER_H


