/* ***** ***** */

#include "memory.h"
#include "heap.h"
#include "freenode.h"
#include "name.h"
#include "normalize.h"
#include "reduce.h"
#include "upcopy.h"

/* ***** ***** */

void memory_setup()
{
    heap_setup();
    cleanup_stack_setup();
    free_stack_setup();
    names_setup();
    norm_stack_setup();
    single_stack_setup();
    upcopy_stack_setup();
}

void memory_free()
{
    heap_free();
    cleanup_stack_free();
    free_stack_free();
    names_free();
    norm_stack_free();
    single_stack_free();
    upcopy_stack_free();
}

/* ***** ***** */

// end of memory.c