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
    llio->streamer_record.blob = (void*)tmp;
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
    llio.top_dir_record.key.obj_name.str = filename;

    // write the "root" header
    char *root = "root";
    root_write(llio.fctx, root, 4);
    llio.location = 4;

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
    llio->header.begin = 100;
    llio->header.units = 4;
    llio->header.compress = 1;
    
    char tmp[300];
    char *buffer = tmp;
    to_buf_file_header(&buffer, &llio->header);
    root_write(llio.fctx, tmp, llio->header.begin);
    llio->location = llio->header.begin;
}

void write_top_dir_record(struct llio_t *llio) {
     llio->top_dir_record.key.version = 4;
     llio->top_dir_record.key.date_time.raw = 1234;
     llio->top_dir_record.key.cycle = 1;
     llio->top_dir_record.key.seek_key = header.begin;
     llio->top_dir_record.key.seek_pdir = 0;
     llio->top_dir_record.key.class_name.str = "TFile";
     llio->top_dir_record.key.class_name.size = 5;
     llio->top_dir_record.key.obj_name.str = filename;
     /*
             52     key.obj_name.size = strlen(filename);
              53     key.obj_title.size = 0;
               54     key.key_bytes = size_key(&key);
                55 
                     56     named.name.str = key.obj_name.str;
                 57     named.name.size = key.obj_name.size;
                  58     named.title.str = key.obj_title.str;
                   59     named.title.size = key.obj_title.size;
                    60 
                         61     dir.version = 5;
                     62     dir.date_time_c.raw = 1234;
                      63     dir.date_time_m.raw = 1235;
                       64     dir.nbytes_name = size_key(&key) + size_named(&named);
                        65     dir.seek_dir = header.begin;
                         66     dir.seek_parent = 0;
                          67 
                               68     // to get the size of bytes for the dir, we need to know the version first
                                69     key.obj_bytes = size_dir(&dir);
                           70     key.total_bytes = key.key_bytes + key.obj_bytes + size_named(&named);

                            73     char *buf_toprec = malloc(key.total_bytes);
                             74     char *buf_toprec_to_free = buf_toprec;
                              75     to_buf_key(&buf_toprec, &key);
                               76     to_buf_named(&buf_toprec, &named);
                                77     to_buf_dir(&buf_toprec, &dir);
                                 78     root_write(ctx, buf_toprec_to_free, key.total_bytes);
                                  79 
                                       80     next_location_to_write += key.total_bytes;
                                   81 
                                        82     dir.seek_keys = next_location_to_write;
                                        */
}
