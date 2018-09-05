#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iolayer.h"

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    struct llio_t llio = open_to_write(filename);

    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes);

    close_from_write(&llio);

    return 0;
}
