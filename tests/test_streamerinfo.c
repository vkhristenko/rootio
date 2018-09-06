#include <string.h>
#include <inttypes.h>

#include "unzip.h"

#include "bootstrap.h"
#include "aux.h"
#include "debug.h"

void test_streamerinfo(struct FileContext ctx, struct TopDirectory const* root) {
    fseek(ctx.pfile, root->header.seek_info, SEEK_SET);

    char *buffer = malloc(root->header.nbytes_info);
    size_t nbytes = fread(buffer, 1, root->header.nbytes_info, ctx.pfile);

    dump_raw(buffer, nbytes, 25);

    struct PKey key_info;
    from_buf_key(&buffer, &key_info);
    print_key(&key_info);


    char *buffer_to_use;
    if (key_info.total_bytes - key_info.key_bytes < key_info.obj_bytes) {
        char *decomp_buffer = malloc(key_info.obj_bytes);
        unzip(&decomp_buffer, buffer, key_info.total_bytes - key_info.key_bytes, key_info.obj_bytes);
        buffer_to_use = decomp_buffer;
    } else
        buffer_to_use = buffer;

    // tlist!
    uint32_t version;
    version = get_version(&buffer_to_use);
    printf("version = %u\n", version);

    version = get_version(&buffer_to_use);
    printf("version = %u\n", version);

    printf("value = %u\n", get_u32(&buffer_to_use)); 
    printf("value = %u\n", get_u32(&buffer_to_use));

    struct PString str;
    from_buf_pstring(&buffer_to_use, &str);
    print_pstring(&str);

    printf("value = %u\n", get_u32(&buffer_to_use));
    printf("value = %u\n", get_u32(&buffer_to_use));
    printf("value = %u\n", get_u32(&buffer_to_use));
    printf("value = %u\n", get_u32(&buffer_to_use));
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
