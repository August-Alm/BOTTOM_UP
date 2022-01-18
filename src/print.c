/* ***** ***** */

#include <stdlib.h>
#include "print.h"
#include "node.h"

/* ***** ***** */

enum print_state_tag {
	P_NORMAL, P_SPACE, P_CLOSING
};

struct print_state {
	node_t node;
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

void fprintf_node(FILE *fp, node_t node)
{
	struct print_state_sll *stack = NULL;
	struct print_state curr = { node, P_NORMAL };
	push_print_state(curr, &stack);

	while (stack) {

		struct print_state curr = pop_print_state(&stack);

		switch (curr.tag) {

		case P_NORMAL: {

			node_t nd = curr.node;

			switch (get_node_kind(nd)) {
			case LEAF_NODE: {
				fprintf(fp, "%s", get_leaf_name(nd));
				continue;
			}
			case SINGLE_NODE: {
				leaf_t l = get_leaf(nd);
				fprintf(fp, "\\%s.", get_leaf_name(l));

				curr.node = get_child(nd);
				curr.tag = P_NORMAL;
				push_print_state(curr, &stack);
				continue;
			}
			case BRANCH_NODE: {
				fprintf(fp, "(");

				curr.node = -1;
				curr.tag = P_CLOSING;
				push_print_state(curr, &stack);

				struct print_state rcurr;
				rcurr.node = get_rchild(nd);
				rcurr.tag = P_NORMAL;
				push_print_state(rcurr, &stack);

                struct print_state dummy;
                dummy.node = -1;
                dummy.tag = P_SPACE;
                push_print_state(dummy, &stack);

				struct print_state lcurr;
				lcurr.node = get_lchild(nd);
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
