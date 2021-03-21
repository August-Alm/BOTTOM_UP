/* ***** ***** */

#ifndef TOKEN_H
#define TOKEN_H

/* ***** ***** */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "forceinline.h"
#include "input.h"

/* ***** ***** */

enum token_tag {
	T_NAME,
	T_LAM,
	T_DOT,
	T_LPAR,
	T_RPAR,
	T_LET,
	T_EQ,
	T_EOF
};

struct token {
	struct name *name;
	uint32_t line;
	uint32_t column;
	enum token_tag tag;
};

FORCEINLINE
struct token mk_token(struct name *nam,
                      uint32_t lin,
                      uint32_t col,
                      enum token_tag tag)
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

bool consume_token(enum token_tag tag, struct input_handle *h);

/* ***** ***** */

void print_token(FILE *fp, struct token tok);

/* ***** ***** */

#endif // TOKEN_H
