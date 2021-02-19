/* ***** ***** */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "basics.h"
#include "parser.h"
#include "context.h"

/* ***** ***** */

void fprintf_term(FILE *out, struct term t)
{
    if (!t.ptr) {
        fprintf(out, "`NULL`-term.");
    }
    switch (KIND(t)) {
    case VAR_TERM:
        fprintf(out, "%s", VAR(t)->name);
        break;
    case LAM_TERM:
        fprintf(out, "\\%s.", LAM(t)->var->name);
        fprintf_term(out, LAM(t)->bod);
        break;
    case APP_TERM:
        fprintf(out, "(");
        fprintf_term(out, APP(t)->fun);
        fprintf(out, " ");
        fprintf_term(out, APP(t)->arg);
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

#define MALCHECKp(s) if(!s) {                              \
    fprintf(stderr, "Malloc failed at line %d in `%s`.\n"  \
                  , __LINE__, __FUNCTION__);               \
    return TERM(NULL);                                     \
}                                                          \

struct term mk_lam(struct var *x, struct term bod)
{
    struct lam *l = lam_halloc();
    if (!l) { printf("Failed lam_halloc.\n"); }
    l->var = x;
    l->bod = bod;
    add_uplink_to(bod, &(l->bod_uplink));
    return TERM(l);
}

struct term mk_app(struct term fun, struct term arg)
{
    struct app *a = app_halloc();
    if (!a) { printf("Failed app_halloc.\n"); }
    a->fun = fun;
    a->arg = arg;
    add_uplink_to(fun, &(a->fun_uplink));
    add_uplink_to(arg, &(a->arg_uplink));
    return TERM(a);
}

struct term parse_term(FILE *inp)
{
    parse_whitespace(inp);
    char c = fgetc(inp);
    if (c == '\\') {
        char *name = malloc(sizeof(char) * 16);
        MALCHECKp(name);
        if (!parse_var(inp, name, 16)) {
            free(name);
            return TERM(NULL);
        }
        if (!parse_char(inp, '.')) {
            free(name);
            return TERM(NULL);
        }
        struct var *newv = var_halloc();
        if (!newv) {
            free(name);
            return TERM(NULL);
        }
        newv->name = name;
        struct term body;
        struct term oldt;
        if (ctx_lookup(name, &oldt)) {
            ctx_swap(name, TERM(newv));
            body = parse_term(inp);
            if (!body.ptr) { return TERM(NULL); }
            ctx_swap(name, oldt);
            free(name);
        }
        else {
            ctx_add(name, TERM(newv));
            body = parse_term(inp);
            if (!body.ptr) { return TERM(NULL); }
        }
        return mk_lam(newv, body);
    }
    if (c == '(') {
        struct term fun = parse_term(inp);
        if (!fun.ptr) {
            return TERM(NULL);
        }
        struct term arg = parse_term(inp);
        if (!arg.ptr) {
            return TERM(NULL);
        }
        if (!parse_char(inp, ')')) {
            return TERM(NULL);
        }
        struct term app = mk_app(fun, arg);
        return app;
    }
    ungetc(c, inp);
    char *name = malloc(sizeof(char) * 16);
    MALCHECKp(name);
    if (!parse_var(inp, name, 16)) {
        free(name);
        return TERM(NULL);
    }
    struct var *x = var_halloc();
    if (!x) {
        free(name);
        return TERM(NULL);
    }
    struct term tx = TERM(x);
    if (ctx_lookup(name, &tx)) {
        free(name);
        return tx;
    }
    x->name = name;
    ctx_add(name, tx);    
    return tx;
}
