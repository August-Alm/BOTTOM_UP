/* ***** ***** */

#include <stdio.h>
#include "src/parser.h"
//#include "src/dag.h"
#include "src/reduction.h"
//#include "src/hmap.h"

/* ***** ***** */

int main(int argc, char *argv[])
{
    if (argc < 2) { return 1; }
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        heap_setup();
        varnames_init();
        
        struct term t = parse_term(fp);
        fprintf_term(stdout, t); printf("\n");

        normalize_wh(t);

        printf("\nAfter normalization:\n");
        fprintf_term(stdout, t);


        memory_free();
        varnames_free();
        fclose(fp);
    }
    else {
        fprintf(stderr, "Error reading file.\n");
        return 1;
    }
    return 0;
}
