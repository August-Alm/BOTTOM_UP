/* ***** ***** */

#ifndef TOKEN_H
#define TOKEN_H

/* ***** ***** */

#include "forceinline.h"
#include "input.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/* ***** ***** */

enum token_tag {
	T_NAME,
	T_LAM,
	T_DOT,
	T_LPAR,
	T_RPAR,
	T_LET,
	T_EQ,
	T_SCLN,
	T_EOF
};

struct token {
	char *name;
	int line;
	int column;
	enum token_tag tag;
};

FORCEINLINE
struct token mk_token(char *nam, int lin, int col, enum token_tag tag)
{
	return (struct token) {
		.name = nam,
		.line = lin,
		.column = col,
		.tag = tag
	};
}

#define PRINT_MSG(str, tok) do {                                     \
	fprintf(stderr, "%s at (%d, %d).\n", str, tok.line, tok.column); \
} while(0)

/* ***** ***** */

struct token read_token(struct input_handle *h);

/* ***** ***** */

void fprintf_token(FILE *fp, struct token tok);

/* ***** ***** */

#endif // TOKEN_H