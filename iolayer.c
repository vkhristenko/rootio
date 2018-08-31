#include "iolayer.h"

struct PBlobRecord root_read_record_blob(struct Context *ctx,
                                         uint64_t location, uint32_t nbytes) {
    char *buffer = malloc(nbytes);
    
    // seek to location and read nbytes
    root_seek(&ctx->fctx, location);
    root_read_buffer(&ctx->fctx, &buffer, nbytes);

    // deserialize key
    struct PBlobRecord blob_rec;
    from_buf_key(&ctx->fctx, &blob_rec.key);
    blob_rec = malloc(blob_rec.key.total_bytes - blob_rec.key.key_bytes);
    memcpy(blob_rec, buffer, )
}

 struct PDirectoryRecord root_read_record_pdirectory(struct Context* ctx,
                                                     uint64_t location, uint32_t nbytes) {
     // allocate room for the whole record
    char *buffer = malloc(nbytes);

    // seek to location and read nbytes
    root_seek(&ctx->fctx, location);
    root_read_buffer(&ctx->fctx, &buffer, nbytes);

    // deserialize
    struct PDirectoryRecord dir_rec;
    from_buf_key(&ctx->fctx, &dir_rec.key);
    from_buf_key(&ctx->fctx, &dir_rec.dir);

    return dir_rec;
}
