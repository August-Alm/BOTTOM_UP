/* ***** ***** */

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "parse.h"
#include "malcheck.h"
#include "name.h"
#include "uplink.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "node.h"
#include "token.h"

/* ***** ***** */

struct env_sll {
    struct env_sll *next;
    int32_t namid;
    node_t bound;
};

static
void add_bound(int32_t nid, node_t nd, struct env_sll **env)
{
    struct env_sll *new = malloc(sizeof(struct env_sll));
    new->next = *env;
    new->namid = nid;
    new->bound = nd;
    *env = new;
}

static
void rem_bound(int32_t nid, node_t bnd, struct env_sll **env)
{
    struct env_sll **tmp = env;
    while (*tmp) {
        if ((*tmp)->namid == nid && (*tmp)->bound == bnd) {
            struct env_sll *next = (*tmp)->next;
            free(*tmp);
            *tmp = next;
            break;
        }
        tmp = &(*tmp)->next;
    }
}

static
node_t get_bound(struct env_sll *env, int32_t nid)
{
    struct env_sll *tmp = env;
    while (tmp) {
        if (tmp->namid == nid) {
            return tmp->bound;
        }
        tmp = tmp->next;
    }
    return -1;
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
        single_t lam_bod;
        branch_t app_arg;
        int32_t let_bnd;
        struct {
             int32_t namid;
             node_t bound;
        } let_bod;
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
        fprintf(stderr, "%s\n", __FUNCTION__);
        exit(EXIT_FAILURE);
    }
    struct state curr = tmp->curr;
    *stack = tmp->next;
    free(tmp);
    return curr;
}

/* ***** ***** */

static inline
void connect_child(node_t ch, single_t s)
{
    set_child(s, ch);
    add_to_parents(get_child_uplink(s), ch);
}

static inline
void connect_lchild(node_t lch, branch_t b)
{
    set_lchild(b, lch);
    add_to_parents(get_lchild_uplink(b), lch);
}

static inline
void connect_rchild(node_t rch, branch_t b)
{
    set_rchild(b, rch);
    add_to_parents(get_rchild_uplink(b), rch);
}

/* ***** ***** */

bool consume_token(struct input_handle *h, enum token_tag tag)
{
	struct token tok = read_token(h);
	if (tok.tag != tag) {
		PRINT_MSG("Unexpected token", tok);
		return false;
	}
	return true;
}

static
node_t parse_env(struct input_handle *h, struct env_sll *env)
{
    struct token tok = read_token(h);
    switch (tok.tag) {
    case T_LAM: {
        tok = read_token(h);
        if (tok.tag != T_NAME) PRINT_MSG("Expected name.", tok);
        int32_t xid = add_name(tok.name);
        single_t s = halloc_single();
        init_single(s, xid);
        leaf_t xvar = get_leaf(s);
        add_bound(xid, xvar, &env);
        consume_token(h, T_DOT);
        node_t body = parse_env(h, env);
        rem_bound(xid, xvar, &env);
        connect_child(body, s);
        return s;
    }
    case T_LPAR: {
        node_t func = parse_env(h, env);
        node_t argm = parse_env(h, env);
        consume_token(h, T_RPAR);
        branch_t b = halloc_branch();
        connect_lchild(func, b);
        connect_rchild(argm, b);
        return b;
    }
    case T_NAME: {
        node_t x = get_bound(env, add_name(tok.name));
        if (x == -1) {
            PRINT_MSG("Free variable.", tok);
        }
        return x;
    }
    case T_LET: {
        tok = read_token(h);
        if (tok.tag != T_NAME) PRINT_MSG("Expected name.", tok);
        int32_t xid = add_name(tok.name);
        consume_token(h, T_EQ);
        node_t bnd = parse_env(h, env);
        consume_token(h, T_SCLN);
        add_bound(xid, bnd, &env);
        node_t res = parse_env(h, env);
        rem_bound(xid, bnd, &env);
        return res;
    }
    default:
        fprintf(stderr, "Inexplicable parse error.");
        return -1;
    }
}

/*
static
node_t parse_env(struct input_handle *h, struct env_sll *env)
{
    node_t retval = -1;

    struct state_sll *stack = NULL;
    struct state curr = (struct state) {
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
                retval = get_bound(env, get_name_id(tok.name));
                continue;
            }
            case T_LAM: {
		        tok = read_token(h);
		        if (tok.tag != T_NAME) {
		            PRINT_MSG("Expected variable name", tok);
		            retval = -1;
                    continue;
		        }
                int32_t nid = add_name(tok.name);
		        tok = read_token(h);
		        if (tok.tag != T_DOT) {
		            PRINT_MSG("Expected '.'", tok);
		            retval = -1;
                    continue;
		        }

                single_t s = halloc_single();
                leaf_t l = get_leaf(s);
                set_leaf_name_id(l, nid);
		        add_binding(nid, l, &env);

                curr.tag = S_LAM_BOD;
                curr.lam_bod = s;
                push_state(curr, &stack);

                struct state newcurr;
                newcurr.tag = S_START;
                push_state(newcurr, &stack);
                continue;
            }
            case T_LPAR: {
                curr.tag = S_APP_FUN;
                push_state(curr, &stack);

                struct state newcurr;
                newcurr.tag = S_START;
                push_state(newcurr, &stack);
                continue;
            }
            case T_LET: {
                tok = read_token(h);
                if (tok.tag != T_NAME) {
                    PRINT_MSG("Expected variable name", tok);
                    retval = -1;
                    continue;
                }
                int32_t nid = add_name(tok.name);
                tok = read_token(h);
                if (tok.tag != T_EQ) {
                    PRINT_MSG("Expected '='", tok);
                    retval = -1;
                    continue;
                }

                curr.tag = S_LET_BND;
                curr.let_bnd = nid;
                push_state(curr, &stack);

                struct state newcurr;
                newcurr.tag = S_START;
                push_state(newcurr, &stack);
                continue;
            }
            default:
                PRINT_MSG("Unexpected token", tok);
                retval = -1;
                continue;
            }
        }

        case S_LAM_BOD: {
            single_t s = curr.lam_bod;
            connect_child(retval, s);
            retval = s;
            leaf_t l = get_leaf(s);
            int32_t nid = get_leaf_id(l);
            rem_binding(nid, l, &env);
            continue;
        }

        case S_APP_FUN: {
            branch_t b = halloc_branch();
            connect_lchild(retval, b);

            curr.tag = S_APP_ARG;
            curr.app_arg = b;
            push_state(curr, &stack);

            struct state newcurr;
            newcurr.tag = S_START;
            push_state(newcurr, &stack);
            continue;
        }

        case S_APP_ARG: {
            branch_t b = curr.app_arg;
            connect_rchild(retval, b);

            tok = read_token(h);
            if (tok.tag != T_RPAR) {
                PRINT_MSG("Expected ')'", tok);
                retval = -1;
                continue;
            }

            retval = b;
            continue;
        }

        case S_LET_BND: {
            int32_t nid = curr.let_bnd;
            node_t bnd = retval;
            add_binding(nid, bnd, &env);

            curr.tag = S_LET_BOD;
            curr.let_bod.namid = nid;
            curr.let_bod.bound = bnd;
            push_state(curr, &stack);

            struct state newcurr;
            newcurr.tag = S_START;
            push_state(newcurr, &stack);
            continue;
        }

        case S_LET_BOD: {
            int32_t nid = curr.let_bod.namid;
            node_t bnd = curr.let_bod.bound;
            rem_binding(nid, bnd, &env);
            continue;
        }

        default:
            fprintf(stderr, "Inexplicable parse error.\n");
            return -1;
        }
    }
    return retval;
}
*/

/* ***** ***** */

node_t parse_node(struct input_handle *h)
{
    struct env_sll *env = NULL;
    node_t result = parse_env(h, env);
    free_env(env);
    return result;
}

/* ***** ***** */

// end of parse.c