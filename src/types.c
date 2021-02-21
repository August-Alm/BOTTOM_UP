/* ***** ***** */

#include "types.h"

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
