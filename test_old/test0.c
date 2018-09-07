#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bootstrap.h"

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    struct FileContext ctx = open_context(filename, "rb");
    dump_contents(ctx);

    close_context(ctx);

    return 0;
}
