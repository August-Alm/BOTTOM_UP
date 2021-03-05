/* ***** ***** */

#include <stddef.h>
#include "uplink.h"
#include "upcopy.h"

/* ***** ***** */

struct upcopy_state *init_upcopy_state(struct branch *redex);

/* ***** ***** */

bool step_upcopy_state(struct upcopy_state *state);

/* ***** ***** */

static
struct node ccache_of(struct uplink *lk);

static
struct node pcache_of(struct uplink *lk);

static
struct single *fresh_single_cache_of(struct single *s);

static
struct branch *fresh_branch_cache_of(struct branch *b);

/* ***** ***** */

void upcopy(struct upcopy_state *st)
{
    if (!st) { return; }
    struct uplink_stack *stk = st->stack;
    struct uplink *curr = st->current;

    while (stk) {
        
        if (!curr) {
            struct uplink *lk = pop(stk);
            if (!lk) { break; }
            st->current = lk;
            st->ccache = ccache_of(lk);
            st->pcache = pcache_of(lk);
        }

        struct node cc = st->ccache;
        struct node pc = st->pcache;

        struct single *s, *cs;
        struct branch *b, *cb;

        if (!pc.address) {
                        
            switch (curr->rel) {
            
            case CHILD_REL:
                s = single_of_child(curr);
                cs = fresh_single_cache_of(s);
                cs->child = cc;
                push_many(s->parents, &stk);
                st->current = NULL;
                break;

            case LCHILD_REL:
                b = branch_of_lchild(curr);
                cb = fresh_branch_cache_of(b);
                cb->lchild = cc;
                push_many(b->parents, &stk);
                st->current = NULL;
                break;

            case RCHILD_REL:
                b = branch_of_rchild(curr);
                cb = fresh_branch_cache_of(b);
                cb->rchild = cc;
                push_many(b->parents, &stk);
                st->current = NULL;
                break;
            }
            continue;
        } 
        
        switch (curr->rel) {
        
        case CHILD_REL:
            s = single_of_child(curr);
            cs = (struct single*)ptr_of(pc.address);
            if (s->id == cs->id) { break; }
            cs->child = cc;
            push_many(s->parents, &stk);
            st->current = NULL;
            break;

        case LCHILD_REL:
            b = branch_of_lchild(curr);
            cb = (struct branch*)ptr_of(pc.address);
            if (s->id == cs->id) { break; }
            cb->lchild = cc;
            push_many(b->parents, &stk);
            st->current = NULL;
            break;

        case RCHILD_REL:
            b = branch_of_rchild(curr);
            cb = (struct branch*)ptr_of(pc.address);
            if (s->id == cs->id) { break; }
            cb->rchild = cc;
            push_many(b->parents, &stk);
            st->current = NULL;
            break;
        } 
    }
}

/* ***** ***** */

// end of upcopy.c
