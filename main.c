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
        
        struct node nd1 = parse_node(fp);
        fprintf_node(stdout, nd1); printf("\n");

        normalize(nd1);
        
        printf("\nAfter normalization:\n");
        fprintf_node(stdout, nd1);


        memory_free();
        fclose(fp);
    }
    else {
        fprintf(stderr, "Error reading file.\n");
        return 1;
    }
    return 0;
}

