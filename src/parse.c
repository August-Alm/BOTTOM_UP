/* ***** ***** */

#include <stdlib.h>
#include <ctype.h>
#include "malcheck.h"
#include "name.h"
#include "parse.h"

/* ***** ***** */

int current_lin = 1;
int current_col = 1;

static inline
int peek_char(FILE *inp)
{
    int c = fgetc(inp);
    if (c != -1) { ungetc(c, inp); }
    return c;
}

static
int pop_char(FILE *inp)
{
    int c = fgetc(inp);
    while (c == '\r' || c == '\n') {
        if (c == '\n') {
            current_lin += 1;
            current_col = 0;
        }
        c = fgetc(inp);
    }
    current_col += 1;
    return c;
}

static inline
int consume_space(FILE *inp)
{
    int c = peek_char(inp);
    while (isspace(c)) {
        pop_char(inp);
        c = peek_char(inp);
    }
    return c;
}

/* ***** ***** */

enum token_tag {
    T_VAR,
    T_LAM,
    T_DOT,
    T_LPAR,
    T_RPAR,
    T_LET,
    T_EQ
};

struct token {
    struct name *name;
    enum token_tag tag;
};

static inline
struct token mk_token(struct name *name, enum token_tag tag)
{
    return (struct token) {
        .name = name,
        .tag = tag
    };
}

/* ***** ***** */

static
struct name *read_name(FILE *inp)
{
    char *str = calloc(1, sizeof(char) * NAME_LEN);
    MALCHECK0(str);
    int c = peek_char(inp);
    int i = 0;
    while (isalpha(c) && i < NAME_LEN) {
        str[i++] = c;
    }
    return get_name(str);
}

static
struct token get_token(FILE *inp)
{
    int c = consume_space(inp);
    if (isalpha(c)) {
        struct name *name = read_name(inp);
        return mk_token(name, T_VAR); 
    }
    switch (c) {
    case '\\':
        pop_char(inp);
        return mk_token(NULL, T_LAM);
    case '.':
        pop_char(inp);
        return mk_token(NULL, T_DOT);
    case '(':
        pop_char(inp);
        return mk_token(NULL, T_LPAR);
    case ')':
        pop_char(inp);
        return mk_token(NULL, T_RPAR);
    case '@':
        pop_char(inp);
        return mk_token(NULL, T_LET);
    case '=':
        pop_char(inp);
        return mk_token(NULL, T_EQ);
    default:
        fprintf(stderr, "Unexpected char = %c at (%d, %d).\n",
                c, current_lin, current_col);
        exit(EXIT_FAILURE);
    }
}

static inline
bool consume_token(enum token_tag tag, FILE *inp)
{
    struct token tok = get_token(inp);
    return tok.tag == tag;
}

/* ***** ***** */

/* ***** ***** */


/* ***** ***** */
