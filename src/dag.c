/* ***** ***** */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "basics.h"
#include "dag.h"

/* ***** ***** */

/* Corresponds to a preallocated heap of 15 MB.
 */
#define HEAP_MAX 39321 // 3932160

/* Daft magic numbers. The maximum number of freed nodes
 * of the respective kind at any given time.
 */
#define VAR_MAX 3932 // 393216 
#define LAM_MAX 3932 // 393216 
#define APP_MAX 3932 // 393216

/* ***** ***** */

int *heap;
int idx_heap;

struct var **var_cleared;
int top_var_cleared;

struct lam **lam_cleared;
int top_lam_cleared;

struct app **app_cleared;
int top_app_cleared;

char** names;
int idx_names;

/* ***** ***** */

void heap_setup()
{
    heap = malloc(sizeof(int) * HEAP_MAX);
    MALCHECKERR(heap);
    idx_heap = 0;

    var_cleared = malloc(sizeof(struct var*) * VAR_MAX);
    MALCHECKERR(var_cleared);
    lam_cleared = malloc(sizeof(struct lam*) * VAR_MAX);
    MALCHECKERR(lam_cleared);
    app_cleared = malloc(sizeof(struct app*) * VAR_MAX);
    MALCHECKERR(app_cleared);

    top_var_cleared = -1;
    top_lam_cleared = -1;
    top_app_cleared = -1;

    names = malloc(sizeof(char*) * VAR_MAX);
    MALCHECKERR(names);
    idx_names = 0;
}

void memory_clear()
{
    idx_heap = 0;
    top_var_cleared = -1;
    top_lam_cleared = -1;
    top_app_cleared = -1;
}

void memory_free()
{
   free(heap);
   free(var_cleared);
   free(lam_cleared); 
   free(app_cleared);
   for (int i = 0; i < idx_names; ++i) {
       free(names[i]);
   }
   free(names);
}

/* ***** ***** */

void store_name(char *name)
{
    if (idx_names == VAR_MAX) {
        fprintf(stderr, "`names` full.\n");
        free(name);
        memory_free();
        exit(EXIT_FAILURE);
    }
    names[idx_names++] = name;
}

/* ***** ***** */

struct var *var_halloc()
{
    if (top_var_cleared == -1) {
        if (idx_heap < HEAP_MAX - sizeof(struct var)) {
            struct var *x = (struct var*)&heap[idx_heap];
            idx_heap += sizeof(struct var);
            x->minus_one = -1;
            x->uplinks = NULL;
            return x;
        }
        fprintf(stderr, "`heap` full.\n");
        return NULL;
    }
    return var_cleared[top_var_cleared--];
}

void var_clear(struct var *var)
{
    if (top_var_cleared < VAR_MAX - 1) {
        var_cleared[++top_var_cleared] = var; 
    }
    else {
        fprintf(stderr, "`var_free` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
}

/* ***** ***** */

struct lam *lam_halloc()
{
    if (top_lam_cleared == -1) {
        if (idx_heap < HEAP_MAX - sizeof(struct lam)) {
            struct lam *l = (struct lam*)&heap[idx_heap];
            idx_heap += sizeof(struct lam);
            l->bod_uplink = (struct uplink_dll) {
                .kind = BOD_UPLINK,
                .next = NULL,
                .prev = NULL
            };
            l->uplinks = NULL;
            return l;
        }
        fprintf(stderr, "`heap` full.\n");
        return NULL;
    }
    printf("Reused lamda.\n");
    return lam_cleared[top_lam_cleared--];
}

void lam_clear(struct lam *lam)
{
    if (top_lam_cleared < LAM_MAX - 1) {
        lam_cleared[++top_lam_cleared] = lam; 
    }
    else {
        fprintf(stderr, "`lam_free` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
}

/* ***** ***** */

struct app *app_halloc()
{
    if (top_app_cleared == -1) {
        if (idx_heap < HEAP_MAX - sizeof(struct app)) {
            struct app *a = (struct app*)&heap[idx_heap];
            idx_heap += sizeof(struct app);
            a->fun_uplink = (struct uplink_dll) {
                .kind = FUN_UPLINK,
                .next = NULL,
                .prev = NULL
            };
            a->arg_uplink = (struct uplink_dll) {
                .kind = ARG_UPLINK,
                .next = NULL,
                .prev = NULL
            };
            a->cache = NULL;
            a->uplinks = NULL;
            return a;
        }
        fprintf(stderr, "`heap` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
    printf("Reused application.\n");
    return app_cleared[top_app_cleared--];
}

void app_clear(struct app *app)
{
    if (top_app_cleared < LAM_MAX - 1) {
        app_cleared[++top_app_cleared] = app; 
    }
    else {
        fprintf(stderr, "`app_free` full.\n");
        memory_free();
        exit(EXIT_FAILURE);
    }
}

/* ***** ***** */
