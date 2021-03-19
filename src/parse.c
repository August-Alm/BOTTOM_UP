/* ***** ***** */

#include <stdlib.h>
#include <ctype.h>
#include "malcheck.h"
#include "name.h"
#include "node.h"
#include "uplink.h"
#include "token.h"
#include "parse.h"

/* ***** ***** */

struct env_sll {
    struct env_sll *next;
    struct name *name;
    struct node bound;
}; 

static
void push_binding(struct name *nam, struct node nd, struct env_sll **env)
{
    struct env_sll *new = malloc(sizeof(struct env_sll));
    new->next = *env;
    new->name = nam;
    new->bound = nd;
    *env = new;
}

static
struct node get_bound(struct env_sll *env, struct name *nam)
{
    struct env_sll *tmp = env;
    while (tmp) {
        if (tmp->name == nam) {
            nam->refcnt++;
            return tmp->bound;
        }
        tmp = tmp->next;
    }
    return as_node(NULL);
}

static
void free_env(struct env_sll *env)
{
    struct env_sll *tmp;
    while (env) {
        tmp = env;
        env = env->next;
        free(tmp);
    }
}

/* ***** ***** */

enum waiting_tag { W_UPLINK, W_NAME };

struct waiting {
    union {
        struct uplink *uplink;
        struct name *name;
    };
    enum waiting_tag tag;
};

struct waiting_sll {
    struct waiting_sll *queue;
    struct waiting first;
};

static
void push_waiting_uplink(struct uplink *lk, struct waiting_sll **ws)
{
    struct waiting_sll *new = malloc(sizeof(struct waiting_sll));
    new->queue = *ws;
    new->first.uplink = lk;
    new->first.tag = W_UPLINK;
    *ws = new;
}

static
void push_waiting_name(struct name *nam, struct waiting_sll **ws)
{
    struct waiting_sll *new = malloc(sizeof(struct waiting_sll));
    new->queue = *ws;
    new->first.name = nam;
    new->first.tag = W_NAME;
    *ws = new;
}

static
struct waiting pop_waiting(struct waiting_sll *ws)
{
    struct waiting w = ws->first;
    struct waiting_sll *tmp = ws;
    *ws = *ws->queue;
    free(tmp);
}

/* ***** ***** */

enum parse_state_tag {
    P_INIT,
    P_RUNNING,
    P_BINDING,
    P_DOT,
    P_RPAR,
    P_EQ,
    P_DONE
};

struct parse_state {
    struct env_sll *env;
    struct waiting_sll *ws;
    struct node res;
    enum parse_state_tag tag;
};

/* ***** ***** */

static inline
void connect_child(struct node nd, struct uplink *lk)
{
    switch (lk->rel) {
    case CHILD_REL:
        single_of_child(lk)->child = nd;
        break;
    case LCHILD_REL:
        branch_of_lchild(lk)->lchild = nd;
        break;
    case RCHILD_REL:
        branch_of_rchild(lk)->rchild = nd;
        break;
    }
    add_to_parents(lk, nd); 
}

static
void serve_waiting(struct parse_state *st, enum waiting_tag wtag, struct node nd)
{
    switch (wtag) {
    case W_UPLINK: {
         struct waiting first = pop_waiting(st->ws);
         struct uplink *lk = first.uplink;
         connect_child(nd, lk);
         if (lk->rel == RCHILD_REL) {
            st->tag = P_RPAR;
            break;
         }
         struct waiting_sll *ws = st->ws;
         if (!ws) {
            st->tag = P_DONE;
         }
         else if (ws->first.tag == W_NAME) {
            st->tag = P_BINDING;
         }
         break;
    }
    case W_NAME: {
        struct waiting first = pop_waiting(st->ws);
        struct name *nam = first.name;
        push_binding(nam, nd, &st->env);
        struct waiting_sll *ws = st->ws;
        if (!ws) {
            st->tag = P_DONE;
        }
        else if (ws->first.tag == W_UPLINK) {
            st->tag = P_RUNNING;
        }
        break;
    }
    default:
        fprintf(stderr, "Inexplicable `serve_waiting` error.\n");
        st->tag = P_DONE;
    }
}

/* ***** ***** */

void p_init(struct parse_state *st, struct token tok)
{

}

void p_running(struct parse_state *st, struct token tok)
{

}

void p_binding(struct parse_state *st, struct token tok)
{

}

void p_dot(struct parse_state *st, struct token tok)
{
    if (tok.tag != T_DOT) {
        PRINT_MSG("Expected '.'", tok);
        st->tag = P_DONE;
    }
}

void p_rpar(struct parse_state *st, struct token tok)
{
    if (tok.tag != T_RPAR) {
        PRINT_MSG("Expected ')'", tok);
        st->tag = P_DONE;
        return;
    }
    struct waiting_sll *ws = st->ws;
    if (!ws) {
        st->tag = P_DONE;
    }
    else if (ws->first.tag == W_UPLINK) {
        st->tag = P_RUNNING;
    }
    else {
        // How to get more than one rpar?
    }
}

void p_eq(struct parse_state *st, struct token tok)
{

}

/* ***** ***** */

/* ***** ***** */

/* ***** ***** */

// end of parse.c

