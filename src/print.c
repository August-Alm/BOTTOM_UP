/* ***** ***** */

#include <stdlib.h>
#include "print.h"
#include "node.h"

/* ***** ***** */

enum print_state_tag {
	P_NORMAL, P_SPACE, P_CLOSING
};

struct print_state {
	struct node node;
	enum print_state_tag tag;
};

struct print_state_sll {
	struct print_state_sll *next;
	struct print_state current;
};

static
void push_print_state(struct print_state cur, struct print_state_sll **stk)
{
    struct print_state_sll *new = malloc(sizeof(struct print_state_sll));
    new->next = *stk;
    new->current = cur;
    *stk = new;
}

static
struct print_state pop_print_state(struct print_state_sll **stk)
{
    if (!stk) {
        fprintf(stderr, "%s.\n", __FUNCTION__);
        exit(EXIT_FAILURE);
    }
    struct print_state_sll *tmp = *stk;
    struct print_state cur = tmp->current;
    *stk = tmp->next;
    free(tmp);
    return cur;
}

/* ***** ***** */

void fprintf_node(FILE *fp, struct node node)
{
	struct print_state_sll *stack = NULL;
	struct print_state curr = { node, P_NORMAL };
	push_print_state(curr, &stack);

	while (stack) {

		struct print_state curr = pop_print_state(&stack);

		switch (curr.tag) {

		case P_NORMAL: {

			struct node nd = curr.node;
			void *nd_ptr = ptr_of(nd.address);

			switch (kind(nd)) {
			case LEAF_NODE: {
				struct leaf *l = (struct leaf*)nd_ptr;
				fprintf(fp, "%s", l->name->str);
				continue;
			}
			case SINGLE_NODE: {
				struct single *s = (struct single*)nd_ptr;
				struct leaf *l = (struct leaf*)ptr_of(s->leaf);
				fprintf(fp, "\\%s.", l->name->str);

				curr.node = s->child;
				curr.tag = P_NORMAL;
				push_print_state(curr, &stack);
				continue;
			}
			case BRANCH_NODE: {
				struct branch *b = (struct branch*)nd_ptr;
				fprintf(fp, "(");

				curr.node = as_node(NULL);
				curr.tag = P_CLOSING;
				push_print_state(curr, &stack);

				struct print_state rcurr;
				rcurr.node = b->rchild;
				rcurr.tag = P_NORMAL;
				push_print_state(rcurr, &stack);

                struct print_state dummy;
                dummy.node = as_node(NULL);
                dummy.tag = P_SPACE;
                push_print_state(dummy, &stack);

				struct print_state lcurr;
				lcurr.node = b->lchild;
				lcurr.tag = P_NORMAL;
				push_print_state(lcurr, &stack);
				continue;
			}
			default:
				fprintf(fp, "Inexplicable fail: unknown node kind.\n");
				continue;
			}
        }
        case P_SPACE: {
            fprintf(fp, " ");
            continue;
        }
		case P_CLOSING:
			fprintf(fp, ")");
			continue;
		}
	}
}

/* ***** ***** */

// end of print.c
