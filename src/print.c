/* ***** ***** */

#include <stdlib.h>
#include "print.h"
#include "node.h"
#include "leaf.h"
#include "single.h"
#include "branch.h"
#include "name.h"

/* ***** ***** */

void fprintf_node(FILE *fp, node_t node)
{
	switch (get_node_kind(node)) {
	case LEAF_NODE:
		fprintf(fp, get_leaf_name(node));
		break;
	case SINGLE_NODE:
		fprintf(fp, "\\");
		fprintf(fp, get_leaf_name(get_leaf(node)));
		fprintf(fp, ".");
		fprintf_node(fp, get_child(node));
		break;
	case BRANCH_NODE:
		fprintf(fp, "(");
		fprintf_node(fp, get_lchild(node));
		fprintf(fp, " ");
		fprintf_node(fp, get_rchild(node));
		fprintf(fp, ")");
		break;
	default:
		break;
	}
}

/* ***** ***** */

// end of print.c
