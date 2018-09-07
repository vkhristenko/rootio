#include <string.h>
#include <inttypes.h>

#include "bootstrap.h"
#include "aux.h"
#include "debug.h"

struct PFree {
    uint64_t begin, end;
};

void from_buf_free(char **buf, struct PFree* ifree) {
    uint32_t version = get_version(buf);
    if (version > 1000) {
        ifree->begin = get_u64(buf);
        ifree->end = get_u64(buf);
    } else {
        ifree->begin = (uint64_t)get_u32(buf);
        ifree->end = (uint64_t)get_u32(buf);
    }
}

void print_free(struct PFree const* pfree) {
    print_u64(pfree->begin);
    print_u64(pfree->end);
}

void test_free_segments(struct FileContext ctx, struct TopDirectory const* root) {
    fseek(ctx.pfile, root->header.seek_free, SEEK_SET);
    
    char *buffer = malloc(root->header.nbytes_free);
    size_t nbytes = fread(buffer, 1, root->header.nbytes_free, ctx.pfile);

    struct PKey key_free;
    from_buf_key(&buffer, &key_free);
    print_key(&key_free);

    for (int i=0; i<root->header.nfree; i++) {
        struct PFree ifree;
        from_buf_free(&buffer, &ifree);
        print_free(&ifree);
    }
        
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf ("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename =%s\n", filename);

    struct FileContext ctx = open_context(filename, "rb");
    struct TopDirectory root = read_top_dir(ctx);

    test_free_segments(ctx, &root);

    close_context(ctx);

    return 0;
}
