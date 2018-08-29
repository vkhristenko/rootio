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

    struct FileContext ctx = open_context(filename, "wb");
    struct PFileHeader header;

    char buffer_array[300];
    char *buffer = buffer_array;
    char *buf_start = buffer;
    memcpy(buffer, "rootroot", 8);
    buffer+=4;

    // put the header into the buffer
    header.version = 61103;
    header.begin = 100;
    header.end = 299;
    header.seek_free = 299;
    header.nbytes_free = 0;
    header.nfree = 0;
    header.units = 4;
    header.compress = 1;
    header.seek_info = 299;
    header.nbytes_info = 0;
    to_buf_file_header(&buffer, &header);

    // key + top dir
    struct PKey key;
    struct PDirectory dir;
    /*
    dir.version = 5;
    dir.date_time_c = {1111111};
    dir.date_time_m = {1111111};
    dir.nbytes_keys = 0;
    dir.nbytes_name = 0;
    dir.seek_dir = header.begin;
    dir.seek_parent = header.begin;
    dir.seek_keys = 0;

    key.total_bytes = sizeof(PDirectory);
    */

    // write the buffer
    root_write(ctx, buf_start, 300);

    /*
    struct KeyList klist = read_keys(ctx, &root.dir);
    for (int i=0; i<klist.size; i++) {
        print_key(&klist.pkeys[i]);
        printf("\n");
 
        char *blob = read_blob(ctx, &klist.pkeys[i]);
        struct PKey dummy_key;
        from_buf_key(&blob, &dummy_key);
      
        if (strcmp(dummy_key.class_name, "TDirectory") == 0) {
            struct PDirectory dir;
            from_buf_dir(&blob, &dir);
            print_dir(&dir);
            printf("\n");

            recurse_and_print(ctx, &dir);
        }
    }*/

    close_context(ctx);

    return 0;
}
