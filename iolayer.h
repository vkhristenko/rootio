#ifndef iolayer_h
#define iolayer_h

#include "bootstrap.h"

struct top_dir_record_t {
    struct PKey key;
    struct PNamed named;
    struct PDirectory dir;
};

struct streamer_record_t {
    struct PKey key;
    void *blob;
};

struct free_segments_record_t {
    struct PKey key;
    int length;
    struct PFree *pfree;
};

struct llio_t {
    uint64_t location;
    struct FileContext fctx;
    struct PFileHeader header;
    struct top_dir_record_t top_dir_rec;
    struct streamer_record_t streamer_record;
    struct free_segments_record_t free_segments_record;
};

//
// api: open/close
//
struct llio_t open_to_read(char *filename);
struct llio_t open_to_write(char *filename);
void close_from_read(struct llio_t*);
void close_from_write(struct llio_t*);

//
// api: read/write records
//
void read_file_header(struct llio_t*);
void read_streamer_record(struct llio_t*);
void read_free_segments_record(struct llio_t*);
void read_top_dir_record(struct llio_t*);

void write_file_header(struct llio_t*);
void write_top_dir_record(struct llio_t*);
void write_streamer_record(struct llio_t*);
void write_free_segments_record(struct llio_t*);
void write_end_byte(struct llio_t*);

#endif // iolayer_h
