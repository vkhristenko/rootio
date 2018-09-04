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

    struct llio_t llio = open_to_read(filename);

    read_file_header(&llio);
    print_file_header(&llio.header);

    printf("--- ---- ---\n");

    read_streamer_record(&llio);
    print_key(&llio.streamer_record.key);

    read_free_segments_record(&llio);
    print_key(&llio.free_segments_record.key);
    printf("nfree = %d\n", llio.free_segments_record.length);
    for (int i=0; i<llio.free_segments_record.length; i++)
        print_pfree(&(llio.free_segments_record.pfree[i]));

    printf("--- ---- ---\n");

    read_top_dir_record(&llio);
    print_key(&llio.top_dir_rec.key);
    printf("--- ---- ---\n");
    print_named(&llio.top_dir_rec.named);
    printf("--- ---- ---\n");
    print_dir(&llio.top_dir_rec.dir);
    printf("--- ---- ---\n");

    close_from_read(&llio);

    return 0;
}
