#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rootio/core/localfs.h"
#include "rootio/core/aux.h"
#include "rootio/core/debug.h"

void localfs_write_to_file(struct localfs_file_t ctx, char const* buf, int size) {
    write(ctx.fd, (void*)buf, size);
}

void localfs_write_to_file_at(struct localfs_file_t ctx, long location, char const*buf,
                              int size) {
    lseek(ctx.fd, location, SEEK_SET);
    write(ctx.fd, (void*)buf, size);
}

struct localfs_file_t localfs_open_file(char const*filename, int flags, int mode) {
    struct localfs_file_t ctx;
    ctx.fd = open(filename, flags, mode);
    return ctx;
}

void localfs_close_file(struct localfs_file_t ctx) {
    close(ctx.fd);
}

void localfs_seek_to(struct localfs_file_t ctx, uint64_t location) {
    lseek(ctx.fd, location, SEEK_SET);
}

void localfs_read_file_header(struct localfs_file_context_t *ctx) {
    char *buffer = malloc(100);
    size_t nbytes = read(ctx->file.fd, (void*)buffer, 100);
    char * tmp = buffer;

    ctor_file_header(&ctx->structure.header);
    from_buf_file_header(&buffer, &ctx->structure.header);

    free(tmp);
}

struct streamer_record_t localfs_read_streamer_record(
        struct localfs_file_context_t *ctx) {
    // seek to the streamer and read key + blob
    localfs_seek_to(ctx->file, ctx->structure.header.seek_info);
    char *buffer = malloc(ctx->structure.header.nbytes_info);
    size_t nbytes = read(ctx->file.fd, (void*)buffer, ctx->structure.header.nbytes_info);
    char *tmp;

    // deser key and assign the blob
    struct streamer_record_t streamer_record;
    from_buf_key(&buffer, &streamer_record.key);
    streamer_record.blob = (char*)buffer;
    return streamer_record;
}

struct free_segments_record_t localfs_read_free_segments_record(
        struct localfs_file_context_t *ctx) {
    // read into the buffer free segments record
    localfs_seek_to(ctx->file, ctx->structure.header.seek_free);
    char *buffer = malloc(ctx->structure.header.nbytes_free);
    size_t nbytes = read(ctx->file.fd, (void*)buffer, ctx->structure.header.nbytes_free);
    char *tmp = buffer;

    // deser key + free[n]
    struct free_segments_record_t free_segments_record;
    from_buf_key(&buffer, &free_segments_record.key);
    free_segments_record.length = ctx->structure.header.nfree;
    free_segments_record.pfree = malloc(free_segments_record.key.obj_bytes);
    struct free_t *ifree = free_segments_record.pfree;
    for (int i=0; i<ctx->structure.header.nfree; i++) {
         from_buf_pfree(&buffer, &(ifree[i]));
    }

    free(tmp);

    return free_segments_record;
}

void localfs_read_top_dir_record(struct localfs_file_context_t *ctx) {
//    fseek(ctx->structure.file.pfile, ctx->structure.header.begin, SEEK_SET);
    localfs_seek_to(ctx->file, ctx->structure.header.begin);
    char *buffer = malloc(ctx->structure.header.nbytes_name);
//    size_t nbytes = fread(buffer, 1, ctx->structure.header.nbytes_name, ctx->structure.file.pfile);
    size_t nbytes = read(ctx->file.fd, (void*)buffer, ctx->structure.header.nbytes_name);
    char *tmp = buffer;

    // deset the key + named
    from_buf_key(&buffer, &ctx->structure.top_dir_rec.key);
    from_buf_named(&buffer, &ctx->structure.top_dir_rec.named);

    // read and deser the directory
//    fseek(ctx->structure.file.pfile, ctx->structure.header.begin+ctx->structure.header.nbytes_name, SEEK_SET);
    localfs_seek_to(ctx->file, ctx->structure.header.begin + ctx->structure.header.nbytes_name);
    struct key_t *key = &ctx->structure.top_dir_rec.key;
    char *buffer_dir = malloc(key->total_bytes - ctx->structure.header.nbytes_name);
//    nbytes = fread(buffer_dir, 1, key->total_bytes - ctx->structure.header.nbytes_name, ctx->structure.file.pfile);
    nbytes = read(ctx->file.fd, (void*)buffer_dir, 
                  key->total_bytes - ctx->structure.header.nbytes_name);
    char *tmp_dir = buffer_dir;
    from_buf_dir(&buffer_dir, &ctx->structure.top_dir_rec.dir);

    free(tmp);
    free(tmp_dir);
}

struct keys_list_record_t localfs_read_keys_list_record_for_dir(struct localfs_file_context_t const*ctx, 
                                                                struct directory_t const*dir) {
    struct keys_list_record_t record;

//    fseek(ctx->structure.file.pfile, dir->seek_keys, SEEK_SET);
    localfs_seek_to(ctx->file, dir->seek_keys);

    // read into memory
    char *buffer = malloc(dir->nbytes_keys);
    char *tmp = buffer;
//    size_t nbytes = fread(buffer, 1, dir->nbytes_keys, ctx->structure.file.pfile);
    size_t nbytes = read(ctx->file.fd, (void*)buffer, dir->nbytes_keys);

    // deset the key + keylist
    from_buf_key(&buffer, &record.key);
    record.length = get_u32(&buffer);
    if (record.length==0) {
        record.pkeys = NULL;
        return record;
    }
    record.pkeys = malloc(sizeof(struct key_t) * record.length);
    for (int i=0; i<record.length; i++)
        from_buf_key(&buffer, &(record.pkeys[i]));

    // postcondition: free resources
    free(tmp);

    return record;
}

struct directory_record_t localfs_read_dir_record_by_key(struct localfs_file_context_t const*ctx, 
                                                         struct key_t const* key) {
    struct directory_record_t record;

//    fseek(ctx->structure.file.pfile, key->seek_key, SEEK_SET);
    localfs_seek_to(ctx->file, key->seek_key);

    // read into mem
    char *buffer = malloc(key->total_bytes);
    char *tmp = buffer;
    //size_t nbytes = fread(buffer, 1, key->total_bytes, ctx->structure.file.pfile);
    size_t nbytes = read(ctx->file.fd, (void*)buffer, key->total_bytes);

    // deser Key + dir
    from_buf_key(&buffer, &record.key);
    from_buf_dir(&buffer, &record.dir);

    // postcondition
    free(tmp);

    return record;
}

struct generic_record_t localfs_read_generic_record_at(struct localfs_file_context_t const*ctx, 
                                                       uint64_t location) {
    struct generic_record_t record;

    // read into mem
    // first read the 4 bytes for total_bytes per record
    // then, the bytes for the record - 4
    localfs_seek_to(ctx->file, location);
    char total_bytes[4];
    char *tmptmp = total_bytes;
    size_t nbytes = read(ctx->file.fd, tmptmp, 4);
    uint32_t nbytes_total = get_u32((char**)&tmptmp);
    char *buffer = malloc(nbytes_total); char *tmp = buffer;
    memcpy(buffer, total_bytes, 4); buffer+=4;
    nbytes = read(ctx->file.fd, (void*)buffer, nbytes_total - 4);
    buffer -= 4;

    // deser the key
    from_buf_key(&buffer, &record.key);
    record.blob = buffer;

    // note we do not deallocate the blob
    return record;
}

struct localfs_file_context_t localfs_open_to_read(char const* filename) {
    struct localfs_file_context_t ctx;
    printf("open to read filename = %s\n", filename);
    ctx.file = localfs_open_file(filename, O_RDONLY, 0);

    /* initialization section */
    localfs_read_file_header(&ctx);
    //localfs_read_streamer_record(&ctx);
    //localfs_read_free_segments_record(&ctx);
    localfs_read_top_dir_record(&ctx);

    return ctx;
}

struct localfs_file_context_t localfs_open_to_write(char const* filename) {
    struct localfs_file_context_t ctx;
    printf("open to write filename = %s\n", filename);
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    ctx.file = localfs_open_file(filename, flags, mode);

    // write the "root" header
    char *root = "root";
    localfs_write_to_file(ctx.file, root, 4);
    ctx.structure.location = 4;

    /* tmp initialization section */
    // header init
    ctor_file_header(&ctx.structure.header);
    
    // top dir record init
    struct string_t class_name, obj_name, title_name;
    class_name.str = "TFile"; class_name.size = 5;
    obj_name.str = filename; obj_name.size = strlen(filename);
    title_name.str = ""; title_name.size = 0;
    ctor_withnames_key(&ctx.structure.top_dir_rec.key, &class_name, &obj_name, &title_name);
    ctor_frompstring_named(&ctx.structure.top_dir_rec.named, &obj_name, &title_name);
    ctor_dir(&ctx.structure.top_dir_rec.dir);

    /* after the initialization above -> sizes of key_t, named_t and directory_t are known */
    /* the only thing directory needs to be in a final state is to also have seek_keys available */
    // top dir record assign the rest of values/sizes
    ctx.structure.top_dir_rec.key.seek_key = ctx.structure.header.begin;
    ctx.structure.top_dir_rec.key.seek_pdir = 0;
    ctx.structure.top_dir_rec.key.key_bytes = size_key(&ctx.structure.top_dir_rec.key);

    ctx.structure.top_dir_rec.dir.nbytes_name = size_key(&ctx.structure.top_dir_rec.key) 
        + size_named(&ctx.structure.top_dir_rec.named);
    ctx.structure.top_dir_rec.dir.seek_dir = ctx.structure.header.begin;
    ctx.structure.top_dir_rec.dir.seek_parent = 0;

    ctx.structure.top_dir_rec.key.obj_bytes = size_dir(&ctx.structure.top_dir_rec.dir) 
        + size_named(&ctx.structure.top_dir_rec.named);
    ctx.structure.top_dir_rec.key.total_bytes = ctx.structure.top_dir_rec.key.key_bytes 
        + ctx.structure.top_dir_rec.key.obj_bytes;
    
    // set the nbytes_name right away for the header
    ctx.structure.header.nbytes_name = ctx.structure.top_dir_rec.dir.nbytes_name;

    // next location to write the record to (does not include header + root dir record)
    ctx.structure.location += ctx.structure.header.begin - 4 + ctx.structure.top_dir_rec.key.total_bytes;

    // write header + root dir record
    localfs_write_file_header(&ctx);
    localfs_write_top_dir_record(&ctx);

    return ctx;
}

void localfs_close_from_read(struct localfs_file_context_t *ctx) {
    printf("closing from read\n");

    localfs_close_file(ctx->file);
}

void localfs_close_from_write(struct localfs_file_context_t *ctx) {
    // actions required at closing time
    //localfs_write_streamer_record(ctx);
    //localfs_write_free_segments_record(ctx);
    localfs_write_end_byte(ctx);
    localfs_write_top_dir_record(ctx);
    localfs_write_file_header(ctx);
    
    // close the file
    localfs_close_file(ctx->file);
}

void localfs_write_file_header(struct localfs_file_context_t* ctx) {
    localfs_seek_to(ctx->file, 4);
//    fseek(ctx->structure.file.pfile, 4, SEEK_SET);

    char tmp[100];
    char *buffer = tmp;
    to_buf_file_header(&buffer, &ctx->structure.header);
    localfs_write_to_file(ctx->file, tmp, ctx->structure.header.begin - 4);
}

void localfs_write_top_dir_record(struct localfs_file_context_t *ctx) {
//    fseek(ctx->structure.file.pfile, ctx->structure.header.begin, SEEK_SET);
    localfs_seek_to(ctx->file, ctx->structure.header.begin);

    char *buffer = malloc(ctx->structure.top_dir_rec.key.total_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &ctx->structure.top_dir_rec.key);
    to_buf_named(&buffer, &ctx->structure.top_dir_rec.named);
    to_buf_dir(&buffer, &ctx->structure.top_dir_rec.dir);
    localfs_write_to_file(ctx->file, tmp, ctx->structure.top_dir_rec.key.total_bytes);

    free(tmp);
}

void localfs_write_streamer_record(struct localfs_file_context_t *ctx,
                                   struct streamer_record_t *streamer_record) {
    localfs_seek_to(ctx->file, ctx->structure.location);
    
    // postcondition
    streamer_record->key.seek_key = ctx->structure.location;

    char *buffer = malloc(streamer_record->key.key_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &streamer_record->key);
    localfs_write_to_file(ctx->file, tmp, streamer_record->key.key_bytes);
    localfs_write_to_file(ctx->file, streamer_record->blob, 
        streamer_record->key.total_bytes - streamer_record->key.key_bytes);

    // postconditions
    ctx->structure.location += streamer_record->key.total_bytes;
    ctx->structure.header.seek_info = streamer_record->key.seek_key;
    ctx->structure.header.nbytes_info = streamer_record->key.total_bytes;

    free(tmp);
}

void localfs_write_free_segments_record(struct localfs_file_context_t *ctx,
                                        struct free_segments_record_t *record) {
    localfs_seek_to(ctx->file, ctx->structure.location);

    // postcondition
    record->key.seek_key = ctx->structure.location;

    char *buffer = malloc(record->key.total_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &record->key);
    for (int i=0; i<record->length; i++)
        to_buf_pfree(&buffer, &(record->pfree[i]));
    localfs_write_to_file(ctx->file, tmp, record->key.total_bytes);

    // postconditions
    ctx->structure.location += record->key.total_bytes;
    ctx->structure.header.nfree = record->length;
    ctx->structure.header.seek_free = record->key.seek_key;
    ctx->structure.header.nbytes_free = record->key.total_bytes;
    free(tmp);
}

void localfs_write_generic_record(struct localfs_file_context_t* ctx, struct generic_record_t *record) {
    //fseek(ctx->structure.file.pfile, ctx->structure.location, SEEK_SET);
    localfs_seek_to(ctx->file, ctx->structure.location);

    // postcondition
    record->key.seek_key = ctx->structure.location;

    // write
    char *buffer = malloc(record->key.key_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &record->key);
    localfs_write_to_file(ctx->file, tmp, record->key.key_bytes);
    localfs_write_to_file(ctx->file, record->blob, record->key.total_bytes - record->key.key_bytes);

    // postcondition
    ctx->structure.location += record->key.total_bytes;

    free(tmp);
}

void localfs_write_end_byte(struct localfs_file_context_t* ctx) {
    // note: no seek is performed -> assuming write_end_byte is called in bundle together with 
    // streamer and free segments records write

    // postcondition
    ctx->structure.header.end = ctx->structure.location;

    // write
    char *end = "v";
//    localfs_write_to_file(ctx->structure.file, end, 1);

    // postcondition
    ctx->structure.location += 1;
}

void localfs_write_keys_list_record_for_dir(struct localfs_file_context_t *ctx, 
                                            struct keys_list_record_t *keys_list_record,
                                            struct directory_t *dir) {
    //fseek(ctx->structure.file.pfile, ctx->structure.location, SEEK_SET);
    localfs_seek_to(ctx->file, ctx->structure.location);

    // postcondition
    keys_list_record->key.seek_key = ctx->structure.location;
    dir->seek_keys = ctx->structure.location;
    dir->nbytes_keys = keys_list_record->key.total_bytes;

    char *buffer = malloc(keys_list_record->key.total_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &keys_list_record->key);
    put_u32(&buffer, (uint32_t)(keys_list_record->length));
    for (int i=0; i<keys_list_record->length; i++)
        to_buf_key(&buffer, &(keys_list_record->pkeys[i]));
    localfs_write_to_file(ctx->file, tmp, keys_list_record->key.total_bytes);

    // postcondition
    ctx->structure.location += keys_list_record->key.total_bytes;
    free(tmp);
}

void localfs_write_directory_record(struct localfs_file_context_t *ctx, struct directory_record_t *dir_record) {
    //fseek(ctx->structure.file.pfile, ctx->structure.location, SEEK_SET);
    localfs_seek_to(ctx->file, ctx->structure.location);

    // postcondition
    dir_record->key.seek_key = ctx->structure.location;

    // deser + write
    char *buffer = malloc(dir_record->key.total_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &dir_record->key);
    to_buf_dir(&buffer, &dir_record->dir);
    localfs_write_to_file(ctx->file, tmp, dir_record->key.total_bytes);

    // postconditions
    ctx->structure.location += dir_record->key.total_bytes;
    free(tmp);
}
