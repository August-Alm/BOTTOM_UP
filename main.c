/* ***** ***** */

#include <stdio.h>
#include "src/memory.h"
#include "src/input.h"
#include "src/parse.h"
#include "src/print.h"

/* ***** ***** */

int main(int argc, char *argv[])
{
    if (argc < 2) { return 1; }
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        memory_setup();
        struct file_handle *fh = new_file_handle(fp);
        node_t nd = parse_node(input_from_file(fh));
        fprintf_node(stdout, nd);
        memory_free();
        free_file_handle(fh);
    }
    else {
        fprintf(stderr, "Error reading file.\n");
        return 1;
    }
    return 0;
}