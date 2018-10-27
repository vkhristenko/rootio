#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rootio/core/iolayer.h"

void recurse(struct localfs_file_context_t *ctx, struct directory_t* dir) {
    struct keys_list_record_t keys_list_record = 
        localfs_read_keys_list_record_for_dir(ctx, dir);
    printf("\n\n--- --- --- ---\n\n");
    printf("*** print a key of the keys list ***\n");
    print_key(&keys_list_record.key);
    
    printf("\n\n--- --- --- ---\n\n");
    printf("*** print a keys list of size = %d\n", keys_list_record.length);
    for (int i=0; i<keys_list_record.length; i++) {
        struct key_t *key = &(keys_list_record.pkeys[i]);
        print_key(key);
        if (strcmp(key->class_name.str, "TDirectory") == 0) {
            struct directory_record_t dir_record = 
                localfs_read_dir_record_by_key(ctx,  key);
            printf("\n\n--- --- --- ---\n\n");
            print_key(&dir_record.key);
            printf("\n\n--- --- --- ---\n\n");
            print_dir(&dir_record.dir);
            recurse(ctx, &dir_record.dir);
        }
    }
}

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    struct localfs_file_context_t ctx = localfs_open_to_read(filename);
    print_file_header(&ctx.structure.header);

    /*
    read_file_header(&ctx->structure.;
    print_file_header(&ctx->structure.header);

    printf("\nprint streamer record\n");
    printf("--- ---- ---\n\n");

    read_streamer_record(&ctx->structure.;
    print_key(&ctx->structure.streamer_record.key);
    
    printf("\nprintf free segments record\n");
    printf("--- ---- ---\n\n");

    read_free_segments_record(&ctx->structure.;
    print_key(&ctx->structure.free_segments_record.key);
    printf("nfree = %d\n", ctx->structure.free_segments_record.length);
    for (int i=0; i<ctx->structure.free_segments_record.length; i++)
        print_pfree(&(ctx->structure.free_segments_record.pfree[i]));

    printf("\nprint top dir record\n");
    printf("--- ---- ---\n\n");

    read_top_dir_record(&ctx->structure.;
    print_key(&ctx->structure.top_dir_rec.key);
    printf("\n--- ---- ---\n\n");
    print_named(&ctx->structure.top_dir_rec.named);
    printf("\n--- ---- ---\n\n");
    print_dir(&ctx->structure.top_dir_rec.dir);

    printf("\n--- ---- ---\n\n");
*/
    recurse(&ctx, &ctx.structure.top_dir_rec.dir);

    /*
    struct keys_list_record_t keys_list_record = read_keys_list_record_for_dir(
        &ctx->structure. &ctx->structure.top_dir_rec.dir);
    print_key(&keys_list_record.key);
    printf("\n--- ---- ---\n\n");
    printf("nkeys = %d\n", keys_list_record.length);
    for (int i=0; i<keys_list_record.length; i++) {
        print_key(&(keys_list_record.pkeys[i]));
        printf("\n--- ---- ---\n\n");
    }*/

    localfs_close_from_read(&ctx);

    return 0;
}
