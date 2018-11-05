#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rootio/core/hdfs/fs.h"
#include "rootio/core/debug.h"

void recurse(struct hdfs_file_context_t *ctx, struct directory_t* dir) {
    struct keys_list_record_t keys_list_record = 
        hdfs_read_keys_list_record_for_dir(ctx, dir);
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
                hdfs_read_dir_record_by_key(ctx, key);
            printf("\n\n--- --- --- ---\n\n");
            print_key(&dir_record.key);
            printf("\n\n--- --- --- ---\n\n");
            print_dir(&dir_record.dir);
            recurse(ctx, &dir_record.dir);
        }
    }
}

void print_key_info(struct key_t *key) {
    printf("At:%10llu N:%10d %15.*s CX = %2.2f\n", key->seek_key, key->total_bytes, 
            key->class_name.size,
            key->class_name.str,
            (float)key->obj_bytes / (float)(key->total_bytes - key->key_bytes));
}

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    struct hdfs_file_context_t ctx = hdfs_open_to_read(filename);

    uint64_t location = ctx.structure.header.begin;
    int irec = 0;
    while (location < ctx.structure.header.end) {
        struct generic_record_t record = 
            hdfs_read_generic_record_at(&ctx, location);
        print_key_info(&record.key);
        char *blob = record.blob;

        // we should retract and free the memory 
        //blob -= record.key.key_bytes;
        blob -= size_key(&record.key);
        free(blob);

        location += record.key.total_bytes;
        irec++;
    }

    hdfs_close_from_read(&ctx);

    return 0;
}
