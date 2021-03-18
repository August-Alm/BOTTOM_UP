/* ***** ***** */

#ifndef PARSE_H
#define PARSE_H

/* ***** ***** */

#include "types.h"

#include <stdlib.h>
#include <stdio.h>


/* ***** ***** */

struct node parse_node(FILE *inp);




/*****************************************************************************/
/*                              Only for testing                             */
/*****************************************************************************/
struct input {
    char *data;
    size_t index;
    size_t peek_index;
    size_t max;
};

struct input *read_file(FILE *inp);
char parse_peek(struct input *inp);
char parse_get(struct input *inp);
/* ***** ***** */

#endif // PARSE_H
