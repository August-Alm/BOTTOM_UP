/* ***** ***** */

#include <stdio.h>
#include "src/heap.h"

/* ***** ***** */

int main(int argc, char *argv[])
{
    if (argc < 2) { return 1; }
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        heap_setup();
        memory_free();
        fclose(fp);
    }
    else {
        fprintf(stderr, "Error reading file.\n");
        return 1;
    }
    return 0;
}
