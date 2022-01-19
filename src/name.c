/* ***** ***** */

#include "malcheck.h"
#include "name.h"
#include "node.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ***** ***** */

#define DEF_SIZE 4096

/* ***** ***** */

char **names;

int count;

/* ***** ***** */

void names_setup()
{
    names = malloc(sizeof(char*) * DEF_SIZE);
    MALCHECKx(names);
    count = 0;
}

void names_free()
{
    if (!names) return;
    for (int i = 0; i < count; ++i) {
        free(names[i]);
    }
    free(names);
    count = 0;
}

int32_t get_name_id(char *str)
{
    for (int i = 0; i < count; ++i) {
        if (!strcmp(names[i], str)) {
            return i;
        }
    }
    return -1;
}

char *get_name(int32_t nid)
{
    if (nid == -1) return NULL;
    return names[nid];
}

int32_t add_name(char *str)
{
    int32_t nid = get_name_id(str);
    if (nid != -1) {
        free(str);
        return nid;
    }
    int32_t idx = count;
    names[idx] = str;
    ++count;
    return idx;
}

/* ***** ***** */

// end of name.c