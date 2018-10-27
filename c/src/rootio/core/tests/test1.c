#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rootio/core/iolayer.h"

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    struct localfs_file_context_t ctx = localfs_open_to_read(filename);

    localfs_read_file_header(&ctx);
    print_file_header(&ctx.structure.header);

    printf("--- ---- ---\n");

    localfs_read_streamer_record(&ctx);
    print_key(&ctx.structure.streamer_record.key);

    localfs_read_free_segments_record(&ctx);
    print_key(&ctx.structure.free_segments_record.key);
    printf("nfree = %d\n", ctx.structure.free_segments_record.length);
    for (int i=0; i<ctx.structure.free_segments_record.length; i++)
        print_pfree(&(ctx.structure.free_segments_record.pfree[i]));

    printf("--- ---- ---\n");

    localfs_read_top_dir_record(&ctx);
    print_key(&ctx.structure.top_dir_rec.key);
    printf("--- ---- ---\n");
    print_named(&ctx.structure.top_dir_rec.named);
    printf("--- ---- ---\n");
    print_dir(&ctx.structure.top_dir_rec.dir);
    printf("--- ---- ---\n");

    localfs_close_from_read(&ctx);

    return 0;
}
