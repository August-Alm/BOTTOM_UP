/* ***** ***** */

#include <stdio.h>
#include "src/heap.h"
#include "src/name.h"
#include "src/input.h"
#include "src/parse.h"
#include "src/print.h"

/* ***** ***** */

int main(int argc, char *argv[])
{
    if (argc < 2) { return 1; }
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        heap_setup();
        setup_names();
        struct file_handle *fh = new_file_handle(fp);
        struct node nd = parse_node(input_from_file(fh));
        fprintf_node(stdout, nd);
        memory_free();
        free_names();
        free_file_handle(fh);
    }
    else {
        fprintf(stderr, "Error reading file.\n");
        return 1;
    }
    return 0;
}
