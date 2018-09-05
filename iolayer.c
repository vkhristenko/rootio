#include "iolayer.h"

void read_file_header(struct llio_t *llio) {
    char *buffer = malloc(100);
    size_t nbytes = fread((void*)buffer, 1, 100, llio->fctx.pfile);
    char * tmp = buffer;

    ctor_file_header(&llio->header);
    from_buf_file_header(&buffer, &llio->header);

    free(tmp);
}

void read_streamer_record(struct llio_t* llio) {
    // seek to the streamer and read key + blob
    fseek(llio->fctx.pfile, llio->header.seek_info, SEEK_SET);
    char *buffer = malloc(llio->header.nbytes_info);
    size_t nbytes = fread(buffer, 1, llio->header.nbytes_info, llio->fctx.pfile);
    char *tmp;

    // deser key and assign the blob
    from_buf_key(&buffer, &llio->streamer_record.key);
    llio->streamer_record.blob = (char*)buffer;
}

void read_free_segments_record(struct llio_t* llio) {
    // read into the buffer free segments record
    fseek(llio->fctx.pfile, llio->header.seek_free, SEEK_SET);
    char *buffer = malloc(llio->header.nbytes_free);
    size_t nbytes = fread(buffer, 1, llio->header.nbytes_free, llio->fctx.pfile);
    char *tmp = buffer;

    // deser key + free[n]
    from_buf_key(&buffer, &llio->free_segments_record.key);
    llio->free_segments_record.length = llio->header.nfree;
    llio->free_segments_record.pfree = malloc(llio->free_segments_record.key.obj_bytes);
    struct PFree *ifree = llio->free_segments_record.pfree;
    for (int i=0; i<llio->header.nfree; i++) {
         from_buf_pfree(&buffer, &(ifree[i]));
    }

    free(tmp);
}

void read_top_dir_record(struct llio_t *llio) {
    fseek(llio->fctx.pfile, llio->header.begin, SEEK_SET);
    char *buffer = malloc(llio->header.nbytes_name);
    size_t nbytes = fread(buffer, 1, llio->header.nbytes_name, llio->fctx.pfile);
    char *tmp = buffer;

    // deset the key + named
    from_buf_key(&buffer, &llio->top_dir_rec.key);
    from_buf_named(&buffer, &llio->top_dir_rec.named);

    // read and deser the directory
    fseek(llio->fctx.pfile, llio->header.begin+llio->header.nbytes_name, SEEK_SET);
    struct PKey *key = &llio->top_dir_rec.key;
    char *buffer_dir = malloc(key->total_bytes - llio->header.nbytes_name);
    nbytes = fread(buffer_dir, 1, key->total_bytes - llio->header.nbytes_name, llio->fctx.pfile);
    char *tmp_dir = buffer_dir;
    from_buf_dir(&buffer_dir, &llio->top_dir_rec.dir);

    free(tmp);
    free(tmp_dir);
}

struct llio_t open_to_read(char *filename) {
    struct llio_t llio;
    llio.fctx = open_context(filename, "rb");
    return llio;
}

struct llio_t open_to_write(char *filename) {
    struct llio_t llio;
    llio.fctx = open_context(filename, "wb");

    // write the "root" header
    char *root = "root";
    root_write(llio.fctx, root, 4);
    llio.location = 4;

    /* tmp initialization section */
    // header init
    llio.header.begin = 100;
    llio.units = 4;
    llio.compress = 1;
    
    // set the name of the file and class name
    llio.top_dir_rec.key.class_name.str = "TFile";
    llio.top_dir_rec.key.class_name.size = strlen("TFile");
    llio.top_dir_rec.key.obj_name.str = filename;
    llio.top_dir_rec.key.obj_name.size = strlen(filename)k;
    llio.top_dir_rec.named.name.str = filename;
    llio.top_dir_rec.named.name.size = llio.top_dir_rec.key.obj_name.size;

    return llio;
}

void close_from_read(struct llio_t *llio) {
    close_context(llio->fctx);
}

void close_from_write(struct llio_t *llio) {
    close_context(llio->fctx);
}

void write_file_header(struct llio_t* llio) {
    fseek(llio->fctx.pfile, 4, SEEK_SET);
    
    char tmp[100];
    char *buffer = tmp;
    to_buf_file_header(&buffer, &llio->header);
    root_write(llio->fctx, tmp, llio->header.begin - 4);
}

void write_top_dir_record(struct llio_t *llio) {
    fseek(llio->fctx.pfile, llio->header.begin, SEEK_SET);

    char *buffer = malloc(llio->top_dir_rec.key.total_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &llio->top_dir_rec.key);
    to_buf_named(&buffer, &llio->top_dir_rec.named);
    to_buf_dir(&buffer, &llio->top_dir_rec.dir);
    root_write(llio->fctx, tmp, llio->top_dir_rec.key.total_bytes);

    free(tmp);
}

void write_streamer_record(struct llio_t *llio) {
    fseek(llio->fctx.pfile, llio->location, SEEK_SET);
    
    // postcondition
    llio->streamer_record.key.seek_key = llio->location;

    char *buffer = malloc(llio->streamer_record.key.key_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &llio->streamer_record.key);
    root_write(llio->fctx, tmp, llio->streamer_record.key.key_bytes);
    root_write(llio->fctx, llio->streamer_record.blob, 
               llio->streamer_record.key.total_bytes - llio->streamer_record.key.key_bytes);

    // postcondition
    llio->location += llio->streamer_record.key.total_bytes;
    free(tmp);
}

void write_free_segments_record(struct llio_t *llio) {
    fseek(llio->fctx.pfile, llio->location, SEEK_SET);

    // postcondition
    llio->free_segments_record.key.seek_key = llio->location;

    char *buffer = malloc(llio->free_segments_record.key.total_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &llio->free_segments_record.key);
    for (int i=0; i<llio->free_segments_record.length; i++)
        to_buf_pfree(&buffer, &(llio->free_segments_record.pfree[i]));
    root_write(llio->fctx, tmp, llio->free_segments_record.key.total_bytes);

    // postcondition
    llio->location += llio->free_segments_record.key.total_bytes;
    free(tmp);
}
