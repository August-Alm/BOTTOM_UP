/* ***** ***** */

#include <ctype.h>
#include <stdlib.h>
#include "malcheck.h"
#include "name.h"
#include "input.h"
#include "token.h"

/* ***** ***** */

extern
struct token mk_token(struct name *nam,
                      uint32_t lin, uint32_t col,
                      enum token_tag tag);

/* ***** ***** */

#define NAME_LEN 12

static
struct name *read_name(char c, struct input_handle *h)
{
    char *str = calloc(1, sizeof(char) * NAME_LEN);
    MALCHECK0(str);
    int i = 0;
    while (isalpha(c) && i < NAME_LEN) {
        str[i++] = c;
        c = read_char(h);
    }
    return get_name(str);
}

/* ***** ***** */

struct token read_token(struct input_handle *h)
{
	int c = read_nonspace_char(h);
    int curr_lin = current_line(h);
    int curr_col = current_column(h);

	if (isalpha(c)) {
		struct name *nam = read_name(c, h);
		return mk_token(nam, curr_lin, curr_col, T_NAME);
	}

	switch (c) {
	case '\\':
		return mk_token(NULL, curr_lin, curr_col, T_LAM);
	case '.':
		return mk_token(NULL, curr_lin, curr_col, T_DOT);
	case '(':
		return mk_token(NULL, curr_lin, curr_col, T_LPAR);
	case ')':
		return mk_token(NULL, curr_lin, curr_col, T_RPAR);
	case '@':
		return mk_token(NULL, curr_lin, curr_col, T_LET);
	case '=':
		return mk_token(NULL, curr_lin, curr_col, T_EQ);
	case -1:
		return mk_token(NULL, curr_lin, curr_col, T_EOF);
	default:
		fprintf(stderr, "Unexpected char = %c at (%d, %d)",
				c, curr_lin, curr_col);
		return mk_token(NULL, curr_lin, curr_col, T_EOF);
	}
}

/* ***** ***** */

bool consume_token(enum token_tag tag, struct input_handle *h)
{
    read_nonspace_char(h);
    int curr_lin = current_line(h);
    int curr_col = current_column(h);

	struct token tok = read_token(h);
	if (tok.tag != tag) {
		fprintf(stderr, "Error at (%d, %d).\n", curr_lin, curr_col);
		return false;
	}
	return true;
}

/* ***** ***** */

// end of token.c