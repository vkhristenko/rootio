#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bootstrap.h"

#include "zlib.h"

void unzip(char **dest, char const* src, int compressed_size) {
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;

    infstream.avail_in = compressed_size;
    infstream.next_in = (Bytef*) src;
    infstream.next_out = (Bytef*) *dest;

    // decompress
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);
}

void recurse_and_print(struct FileContext ctx, struct PDirectory const* pdir) {
    struct KeyList klist = read_keys(ctx, pdir);
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
        } else {
            char *object_blob = blob;
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

    struct FileContext ctx = open_context(filename, "rb");

    struct TopDirectory root = read_top_dir(ctx);
    print_file_header(&root.header);
    printf("\n");
    print_dir(&root.dir);
    printf("\n");

    recurse_and_print(ctx, &root.dir);

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
