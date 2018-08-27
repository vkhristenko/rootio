#include <string.h>
#include <inttypes.h>

#include "unzip.h"

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

void test_streamerinfo(struct FileContext ctx, struct TopDirectory const* root) {
    fseek(ctx.pfile, root->header.seek_info, SEEK_SET);

    char *buffer = malloc(root->header.nbytes_info);
    size_t nbytest = fread(buffer, 1, root->header.nbytes_info, ctx.pfile);

    struct PKey key_info;
    from_buf_key(&buffer, &key_info);
    print_key(&key_info);

    char *decomp_buffer = malloc(key_info.obj_bytes);
    unzip(&decomp_buffer, buffer, key_info.total_bytes - key_info.key_bytes, key_info.obj_bytes);

    // tlist!
    uint32_t version;
    version = get_version(&decomp_buffer);
    printf("version = %u\n", version);

    version = get_version(&decomp_buffer);
    printf("version = %u\n", version);

    printf("value = %u\n", get_u32(&decomp_buffer)); 
    printf("value = %u\n", get_u32(&decomp_buffer));

    char *str;
    get_string(&decomp_buffer, &str);
    printf("string = %s\n", str);

    printf("value = %u\n", get_u32(&decomp_buffer));
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
    print_file_header(&root.header);

    test_streamerinfo(ctx, &root);

    close_context(ctx);

    return 0;
}
