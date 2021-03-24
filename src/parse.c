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
void add_binding(struct name *nam, struct node nd, struct env_sll **env)
{
    struct env_sll *new = malloc(sizeof(struct env_sll));
    new->next = *env;
    new->name = nam;
    new->bound = nd;
    *env = new;
}

static
void rem_binding(struct name *nam, struct node bnd, struct env_sll **env)
{
    struct env_sll **tmp = env;
    while (*tmp) {
        if ((*tmp)->name == nam && (*tmp)->bound.address == bnd.address) {
            struct env_sll *next = (*tmp)->next;
            free(*tmp);
            *tmp = next;
            break;
        }
        tmp = &(*tmp)->next;
    }
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

enum state_tag {
    S_START,
    S_LAM_BOD,
    S_APP_FUN,
    S_APP_ARG,
    S_LET_BND,
    S_LET_BOD
};

struct state {
    union {
        struct {
             struct name *name;
             struct leaf *leaf;
        } lam_bod;
        struct branch *app_arg;
        struct name *let_bnd;
        struct {
             struct name *name;
             struct node bound;
        } let_bod;
        void *junk;
    };
    enum state_tag tag;
};

struct state_sll {
    struct state_sll *next;
    struct state curr;
};

void push_state(struct state newcurr, struct state_sll **stack)
{
    struct state_sll *new = malloc(sizeof(struct state_sll));
    new->next = *stack;
    new->curr = newcurr;
    *stack = new;
}

struct state pop_state(struct state_sll **stack)
{
    struct state_sll *tmp = *stack;
    if (!tmp) {
        fprintf(stderr, "Can't pop empty state stack!\n");
        exit(EXIT_FAILURE);
    }
    struct state curr = tmp->curr;
    *stack = tmp->next;
    free(tmp);
    return curr;
}

/* ***** ***** */

static inline
void connect_child(struct node ch, struct single *s)
{
    s->child = ch;
    add_to_parents(&s->child_uplink, ch);
}

static inline
void connect_lchild(struct node lch, struct branch *b)
{
    b->lchild = lch;
    add_to_parents(&b->lchild_uplink, lch);
}

static inline
void connect_rchild(struct node rch, struct branch *b)
{
    b->rchild = rch;
    add_to_parents(&b->rchild_uplink, rch);
}

/* ***** ***** */

static
struct node parse_env(struct input_handle *h, struct env_sll *env)
{
    struct node retval = as_node(NULL);

    struct state_sll *stack = NULL;
    struct state curr = (struct state) {
        .junk = NULL,
        .tag = S_START
    };
    push_state(curr, &stack);

    struct token tok;

    while (stack) {
        curr = pop_state(&stack);

        switch (curr.tag) {

        case S_START: {
            tok = read_token(h);
            switch (tok.tag) {
            case T_NAME: {
                retval = get_bound(env, tok.name);
                continue;
            }
            case T_LAM: {
		        tok = read_token(h);

		        if (tok.tag != T_NAME) {
		            PRINT_MSG("Expected variable name", tok);
		            retval = as_node(NULL);
                    continue;
		        }
		        struct name *nam = tok.name;
		        tok = read_token(h);
		        if (tok.tag != T_DOT) {
		            PRINT_MSG("Expected '.'", tok);
		            retval = as_node(NULL);
                    continue;
		        }
		        struct leaf *l = halloc_leaf();
		        l->name = nam;
		        add_binding(nam, as_node(l), &env);

                curr.tag = S_LAM_BOD;
                curr.lam_bod.name = nam;
                curr.lam_bod.leaf = l;
                push_state(curr, &stack);

                struct state newcurr;
                newcurr.tag = S_START;
                newcurr.junk = NULL;
                push_state(newcurr, &stack);
                continue;
            }
            case T_LPAR: {
                curr.tag = S_APP_FUN;
                curr.junk = NULL;
                push_state(curr, &stack);

                struct state newcurr;
                newcurr.tag = S_START;
                newcurr.junk = NULL;
                push_state(newcurr, &stack);
                continue;
            }
            case T_LET: {
                tok = read_token(h);
                if (tok.tag != T_NAME) {
                    PRINT_MSG("Expected variable name", tok);
                    retval = as_node(NULL);
                    continue;
                }
                struct name *nam = tok.name;
                tok = read_token(h);
                if (tok.tag != T_EQ) {
                    PRINT_MSG("Expected '='", tok);
                    retval = as_node(NULL);
                    continue;
                }

                curr.tag = S_LET_BND;
                curr.let_bnd = nam;
                push_state(curr, &stack);

                struct state newcurr;
                newcurr.tag = S_START;
                newcurr.junk = NULL;
                push_state(newcurr, &stack);
                continue;
            }
            default:
                PRINT_MSG("Unexpected token", tok);
                retval = as_node(NULL);
                continue;
            }
        }

        case S_LAM_BOD: {
            struct single *s = halloc_single();
            struct leaf *l = curr.lam_bod.leaf;
            s->leaf = address_of(l);
            connect_child(retval, s);
            retval = as_node(s);
            rem_binding(l->name, as_node(l), &env);
            continue;
        }

        case S_APP_FUN: {
            struct branch *b = halloc_branch();
            connect_lchild(retval, b);

            curr.tag = S_APP_ARG;
            curr.app_arg = b;
            push_state(curr, &stack);

            struct state newcurr;
            newcurr.tag = S_START;
            newcurr.junk = NULL;
            push_state(newcurr, &stack);
            continue;
        }

        case S_APP_ARG: {
            struct branch *b = curr.app_arg;
            connect_rchild(retval, b);

            tok = read_token(h);
            if (tok.tag != T_RPAR) {
                PRINT_MSG("Expected ')'", tok);
                retval = as_node(NULL);
                continue;
            }

            retval = as_node(b);
            continue;
        }

        case S_LET_BND: {
            struct name *nam = curr.let_bnd;
            struct node bnd = retval;
            add_binding(nam, bnd, &env);

            curr.tag = S_LET_BOD;
            curr.let_bod.name = nam;
            curr.let_bod.bound = bnd;
            push_state(curr, &stack);

            struct state newcurr;
            newcurr.tag = S_START;
            newcurr.junk = NULL;
            push_state(newcurr, &stack);
            continue;
        }

        case S_LET_BOD: {
            struct name *nam = curr.let_bod.name;
            struct node bnd = curr.let_bod.bound;
            rem_binding(nam, bnd, &env);
            continue;
        }

        default:
            fprintf(stderr, "Inexplicable parse error.\n");
            return as_node(NULL);
        }
    }
    return retval;
}

/* ***** ***** */

struct node parse_node(struct input_handle *h)
{
    struct env_sll *env = NULL;
    struct node result = parse_env(h, env);
    free_env(env);
    return result;
}

/* ***** ***** */

// end of parse.c
