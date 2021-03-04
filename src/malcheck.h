/* ***** ***** */

#ifndef MALCHECK_H
#define MALCHECK_H

/* ***** ***** */

#include <stdio.h>
#include <stdbool.h>

/* ***** ***** */

#define MALCHECK0(s) if(!s) {                              \
    fprintf(stderr, "Malloc failed at line %d in `%s`.\n"  \
                  , __LINE__, __FUNCTION__);               \
    return NULL;                                           \
}                                                          \

#define MALCHECKb(s) if(!s) {                              \
    fprintf(stderr, "Malloc failed at line %d in `%s`.\n"  \
                  , __LINE__, __FUNCTION__);               \
    return false;                                          \
}                                                          \

#define MALCHECKx(s) if(!s) {                              \
    fprintf(stderr, "Malloc failed at line %d in `%s`.\n"  \
                  , __LINE__, __FUNCTION__);               \
    exit(EXIT_FAILURE);                                    \
}                                                          \

/* ***** ***** */

#endif // MALCHECK_H
