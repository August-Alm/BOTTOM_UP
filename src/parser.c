/* ***** ***** */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "basics.h"
#include "parser.h"

/* ***** ***** */

void fprintf_node(FILE *out, struct node nd)
{
    if (!nd.term) {
        fprintf(out, "`NULL`-term.");
    }
    switch (KIND(nd)) {
    case VAR_NODE:
        fprintf(out, "%s", VAR(nd)->name);
        break;
    case LAM_NODE:
        fprintf(out, "\\%s.", LAM(nd)->var->name);
        fprintf_node(out, LAM(nd)->bod);
        break;
    case APP_NODE:
        fprintf(out, "(");
        fprintf_node(out, APP(nd)->fun);
        fprintf(out, " ");
        fprintf_node(out, APP(nd)->arg);
        fprintf(out, ")");
        break;
    }
}

/* ***** ***** */

//  Parsing utility functions

//  Returns `0` if it the current char of `inp` is not `chr`, `1` if it is.
int parse_char(FILE *inp, char chr)
{
    int c = fgetc(inp);
    if (c == EOF) {
        fprintf(stderr, "Unexpected EOF during `parse_char`.\n");
        return 0;
    } else if (c != chr) {
        ungetc(c, inp);
        fprintf(stderr, "Bad char; expected '%c' but got '%c'.\n", chr, c);
        return 0;
    }
    return 1;
}

//  Returns `0` if it fails, `1` if it succeeds. Expects `buf` to be an
//  uninitialized (but allocated) string of length `sz`.
int parse_var(FILE *inp, char* buf, size_t sz)
{
    char c = fgetc(inp);
    if (c == EOF) {
        fprintf(stderr, "Unexpected EOF during `parse_name`.\n");
        return 0;
    }
    int i = 0;
    while ((isalnum(c) || c == '_') && i < sz - 1) {
        buf[i] = c;
        buf[i+1] = '\0';
        c = fgetc(inp);
        if (c == EOF) {
            fprintf(stderr, "Unexpected EOF during `parse_name`.\n");
            return 0;
        }
        i++;
        if (i == sz - 1) {
            char c = fgetc(inp);
            if (isalnum(c) || c == '_') {
                fprintf(stderr, "Too long variable during `parse_name`.");
                return 0;
            } else {
                ungetc(c, inp);
            }
        }
    }    
    ungetc(c, inp);
    return 1;
}

//  Advances the current char of `inp` until it is not a white-space.
void parse_whitespace(FILE *inp)
{
    int c = fgetc(inp);
    if (isspace(c)) {
        parse_whitespace(inp);
    } else {
        ungetc(c, inp);
    }
}

/* ***** ***** */

const struct node null_node = { .term = NULL };

#define MALCHECKP(s) if(!s) {                              \
    fprintf(stderr, "Malloc failed at line %d in `%s`.\n"  \
                  , __LINE__, __FUNCTION__);               \
    return null_node;                                      \
}                                                          \

struct node mk_lam(struct var *x, struct node bod)
{
    struct lam *l = lam_halloc();
    if (!l) { printf("failed lam_halloc\n"); }
    l->var = x;
    l->bod = bod;
    add_uplink_to(bod, &(l->bod_uplink));
    return (struct node) { .term = l };
}

struct node mk_app(struct node fun, struct node arg)
{
    struct app *a = app_halloc();
    if (!a) { printf("failed app_halloc.\n"); }
    a->fun = fun;
    a->arg = arg;
    add_uplink_to(fun, &(a->fun_uplink));
    add_uplink_to(arg, &(a->arg_uplink));
    return (struct node) { .term = a };
}

struct node parse_node(FILE *inp)
{
    parse_whitespace(inp);
    char c = fgetc(inp);
    if (c == '\\') {
        char *name = malloc(sizeof(char) * 16);
        MALCHECKP(name);
        if (!parse_var(inp, name, 16)) {
            free(name);
            return null_node;
        }
        if (!parse_char(inp, '.')) {
            free(name);
            return null_node;
        }
        struct var *x = new_var(name);
        struct node body = parse_node(inp);
        if (!body.term) { return null_node; }
        return mk_lam(x, body);
    }
    if (c == '(') {
        struct node function = parse_node(inp);
        if (!function.term) { return null_node; }
        struct node argument = parse_node(inp);
        if (!argument.term) { return null_node; }
        if (!parse_char(inp, ')')) { return null_node; }
        struct node application = mk_app(function, argument);
        return application;
    }
    ungetc(c, inp);
    char *name = malloc(sizeof(char) * 16);
    MALCHECKP(name);
    if (!parse_var(inp, name, 16)) {
        free(name);
        return null_node;
    }
    struct var *variable = new_var(name);
    return (struct node) { .term = variable };
}
