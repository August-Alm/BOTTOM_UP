/* ***** ***** */

#include <stdio.h>
#include "src/parser.h"
#include "src/dag.h"
#include "src/reduction.h"

/* ***** ***** */

int main(int argc, char *argv[])
{
    if (argc < 2) { return 1; }
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        heap_setup();
        
        struct node nd = parse_node(fp);
        fprintf_node(stdout, nd); printf("\n");

        normalize_wh(nd);

        printf("\nAfter normalization:\n");
        fprintf_node(stdout, nd);


        memory_free();
        fclose(fp);
    }
    else {
        fprintf(stderr, "Error reading file.\n");
        return 1;
    }
    return 0;
}

