#include <string.h>

#include "iolayer.h"
#include "aux.h"

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
    struct free_t *ifree = llio->free_segments_record.pfree;
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
    struct key_t *key = &llio->top_dir_rec.key;
    char *buffer_dir = malloc(key->total_bytes - llio->header.nbytes_name);
    nbytes = fread(buffer_dir, 1, key->total_bytes - llio->header.nbytes_name, llio->fctx.pfile);
    char *tmp_dir = buffer_dir;
    from_buf_dir(&buffer_dir, &llio->top_dir_rec.dir);

    free(tmp);
    free(tmp_dir);
}

struct keys_list_record_t read_keys_list_record_for_dir(struct llio_t *llio, struct directory_t *dir) {
    struct keys_list_record_t record;

    fseek(llio->fctx.pfile, dir->seek_keys, SEEK_SET);

    // read into memory
    char *buffer = malloc(dir->nbytes_keys);
    char *tmp = buffer;
    size_t nbytes = fread(buffer, 1, dir->nbytes_keys, llio->fctx.pfile);

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

struct directory_record_t read_dir_record_by_key(struct llio_t *llio, struct key_t *key) {
    struct directory_record_t record;

    fseek(llio->fctx.pfile, key->seek_key, SEEK_SET);

    // read into mem
    char *buffer = malloc(key->total_bytes);
    char *tmp = buffer;
    size_t nbytes = fread(buffer, 1, key->total_bytes, llio->fctx.pfile);

    // deser Key + dir
    from_buf_key(&buffer, &record.key);
    from_buf_dir(&buffer, &record.dir);

    // postcondition
    free(tmp);

    return record;
}

struct generic_record_t read_generic_record_by_location(struct llio_t *llio, uint64_t location) {
    struct generic_record_t record;

    // read into mem
    // first read the 4 bytes for total_bytes per record
    // then, the bytes for the record - 4
    fseek(llio->fctx.pfile, location, SEEK_SET);
    char total_bytes[4];
    char *tmptmp = total_bytes;
    size_t nbytes = fread(tmptmp, 1, 4, llio->fctx.pfile);
    uint32_t nbytes_total = get_u32((char**)&tmptmp);
    char *buffer = malloc(nbytes_total); char *tmp = buffer;
    memcpy(buffer, total_bytes, 4); buffer+=4;
    nbytes = fread(buffer, 1, nbytes_total - 4, llio->fctx.pfile);
    buffer -= 4;

    // deser the key
    from_buf_key(&buffer, &record.key);
    record.blob = buffer;

    // note we do not deallocate the blob
    return record;
}

struct llio_t open_to_read(char *filename) {
    struct llio_t llio;
    printf("open to read filename = %s\n", filename);
    llio.fctx = open_context(filename, "rb");

    /* initialization section */
    read_file_header(&llio);
    read_streamer_record(&llio);
    read_free_segments_record(&llio);
    read_top_dir_record(&llio);

    return llio;
}

struct llio_t open_to_write(char *filename) {
    struct llio_t llio;
    printf("open to write filename = %s\n", filename);
    llio.fctx = open_context(filename, "wb");

    // write the "root" header
    char *root = "root";
    root_write(llio.fctx, root, 4);
    llio.location = 4;

    /* tmp initialization section */
    // header init
    ctor_file_header(&llio.header);
    
    // top dir record init
    struct string_t class_name, obj_name, title_name;
    class_name.str = "TFile"; class_name.size = 5;
    obj_name.str = filename; obj_name.size = strlen(filename);
    title_name.str = ""; title_name.size = 0;
    ctor_withnames_key(&llio.top_dir_rec.key, &class_name, &obj_name, &title_name);
    ctor_frompstring_named(&llio.top_dir_rec.named, &obj_name, &title_name);
    ctor_dir(&llio.top_dir_rec.dir);

    /* after the initialization above -> sizes of key_t, named_t and directory_t are known */
    /* the only thing directory needs to be in a final state is to also have seek_keys available */
    // top dir record assign the rest of values/sizes
    llio.top_dir_rec.key.seek_key = llio.header.begin;
    llio.top_dir_rec.key.seek_pdir = 0;
    llio.top_dir_rec.key.key_bytes = size_key(&llio.top_dir_rec.key);

    llio.top_dir_rec.dir.nbytes_name = size_key(&llio.top_dir_rec.key) + size_named(&llio.top_dir_rec.named);
    llio.top_dir_rec.dir.seek_dir = llio.header.begin;
    llio.top_dir_rec.dir.seek_parent = 0;

    llio.top_dir_rec.key.obj_bytes = size_dir(&llio.top_dir_rec.dir) + size_named(&llio.top_dir_rec.named);
    llio.top_dir_rec.key.total_bytes = llio.top_dir_rec.key.key_bytes + llio.top_dir_rec.key.obj_bytes;
    
    // set the nbytes_name right away for the header
    llio.header.nbytes_name = llio.top_dir_rec.dir.nbytes_name;

    // next location to write the record to (does not include header + root dir record)
    llio.location += llio.header.begin - 4 + llio.top_dir_rec.key.total_bytes;

    // write header + root dir record
    write_file_header(&llio);
    write_top_dir_record(&llio);

    return llio;
}

void close_from_read(struct llio_t *llio) {
    printf("closing from read\n");

    close_context(llio->fctx);
}

void close_from_write(struct llio_t *llio) {
    // actions required at closing time
    write_streamer_record(llio);
    write_free_segments_record(llio);
    write_end_byte(llio);
    write_top_dir_record(llio);
    write_file_header(llio);
    
    // close the file
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

    // postconditions
    llio->location += llio->streamer_record.key.total_bytes;
    llio->header.seek_info = llio->streamer_record.key.seek_key;
    llio->header.nbytes_info = llio->streamer_record.key.total_bytes;

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

    // postconditions
    llio->location += llio->free_segments_record.key.total_bytes;
    llio->header.nfree = llio->free_segments_record.length;
    llio->header.seek_free = llio->free_segments_record.key.seek_key;
    llio->header.nbytes_free = llio->free_segments_record.key.total_bytes;
    free(tmp);
}

void write_generic_record(struct llio_t* llio, struct generic_record_t *record) {
    fseek(llio->fctx.pfile, llio->location, SEEK_SET);

    // postcondition
    record->key.seek_key = llio->location;

    // write
    char *buffer = malloc(record->key.key_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &record->key);
    root_write(llio->fctx, tmp, record->key.key_bytes);
    root_write(llio->fctx, record->blob, record->key.total_bytes - record->key.key_bytes);

    // postcondition
    llio->location += record->key.total_bytes;

    free(tmp);
}

void write_end_byte(struct llio_t* llio) {
    // note: no seek is performed -> assuming write_end_byte is called in bundle together with 
    // streamer and free segments records write

    // postcondition
    llio->header.end = llio->location;

    // write
    char *end = "v";
//    root_write(llio->fctx, end, 1);

    // postcondition
    llio->location += 1;
}

void write_keys_list_record_for_dir(struct llio_t *llio, struct keys_list_record_t *keys_list_record,
                                    struct directory_t *dir) {
    fseek(llio->fctx.pfile, llio->location, SEEK_SET);

    // postcondition
    keys_list_record->key.seek_key = llio->location;
    dir->seek_keys = llio->location;
    dir->nbytes_keys = keys_list_record->key.total_bytes;

    char *buffer = malloc(keys_list_record->key.total_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &keys_list_record->key);
    put_u32(&buffer, (uint32_t)(keys_list_record->length));
    for (int i=0; i<keys_list_record->length; i++)
        to_buf_key(&buffer, &(keys_list_record->pkeys[i]));
    root_write(llio->fctx, tmp, keys_list_record->key.total_bytes);

    // postcondition
    llio->location += keys_list_record->key.total_bytes;
    free(tmp);
}

void write_directory_record(struct llio_t *llio, struct directory_record_t *dir_record) {
    fseek(llio->fctx.pfile, llio->location, SEEK_SET);

    // postcondition
    dir_record->key.seek_key = llio->location;

    // deser + write
    char *buffer = malloc(dir_record->key.total_bytes);
    char *tmp = buffer;
    to_buf_key(&buffer, &dir_record->key);
    to_buf_dir(&buffer, &dir_record->dir);
    root_write(llio->fctx, tmp, dir_record->key.total_bytes);

    // postconditions
    llio->location += dir_record->key.total_bytes;
    free(tmp);
}
