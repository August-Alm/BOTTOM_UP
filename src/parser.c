/* ***** ***** */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "basics.h"
#include "parser.h"
#include "hmap.h"

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

const struct term null_term = { .ptr = NULL };

#define MALCHECKP(s) if(!s) {                              \
    fprintf(stderr, "Malloc failed at line %d in `%s`.\n"  \
                  , __LINE__, __FUNCTION__);               \
    return null_term;                                      \
}                                                          \

struct term mk_lam(struct var *x, struct term bod)
{
    struct lam *l = lam_halloc();
    if (!l) { printf("Failed lam_halloc.\n"); }
    l->var = x;
    l->bod = bod;
    add_uplink_to(bod, &(l->bod_uplink));
    return (struct term) { .ptr = l };
}

struct term mk_app(struct term fun, struct term arg)
{
    struct app *a = app_halloc();
    if (!a) { printf("Failed app_halloc.\n"); }
    a->fun = fun;
    a->arg = arg;
    add_uplink_to(fun, &(a->fun_uplink));
    add_uplink_to(arg, &(a->arg_uplink));
    return (struct term) { .ptr = a };
}

struct term parse_term(FILE *inp)
{
    parse_whitespace(inp);
    char c = fgetc(inp);
    if (c == '\\') {
        char *name = calloc(1, sizeof(char) * 16);
        MALCHECKP(name);
        if (!parse_var(inp, name, 16)) {
            free(name);
            return null_term;
        }
        if (!parse_char(inp, '.')) {
            free(name);
            return null_term;
        }
        struct var *x = var_halloc();
        if (!x) {
            free(name);
            memory_free();
        }
        if (!varnames_lookup(name, &x)) {
            x->name = name;
            varnames_add(name, x);    
        }
        struct term body = parse_term(inp);
        if (!body.ptr) { return null_term; }
        return mk_lam(x, body);
    }
    if (c == '(') {
        struct term function = parse_term(inp);
        if (!function.ptr) { return null_term; }
        struct term argument = parse_term(inp);
        if (!argument.ptr) { return null_term; }
        if (!parse_char(inp, ')')) { return null_term; }
        struct term application = mk_app(function, argument);
        return application;
    }
    ungetc(c, inp);
    char *name = calloc(1, sizeof(char) * 16);
    MALCHECKP(name);
    if (!parse_var(inp, name, 16)) {
        free(name);
        return null_term;
    }
    struct var *x = var_halloc();
    if (!x) {
        free(name);
        memory_free();
    }
    if (!varnames_lookup(name, &x)) {
        x->name = name;
        varnames_add(name, x);    
    }
    return (struct term) { .ptr = x };
}
