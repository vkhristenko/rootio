#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rootio/core/localfs.h"

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
    recurse(&ctx, &ctx.structure.top_dir_rec.dir);
    localfs_close_from_read(&ctx);

    return 0;
}
