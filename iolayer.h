#ifndef iolayer_h
#define iolayer_h

#include "bootstrap.h"

struct top_dir_record_t {
    struct rkey_t key;
    struct rnamed_t named;
    struct rdirectory_t dir;
};

struct directory_record_t {
    struct rkey_t key;
    struct rdirectory_t dir;
};

struct keys_list_record_t {
    struct rkey_t key;
    int length;
    struct rkey_t *pkeys;
};

struct generic_record_t {
    struct rkey_t key;
    char *blob;
};

/**
 * Streamer Record. For now a simple impl
 *   Key + TList of StreamerElements
 */
struct streamer_record_t {
    struct rkey_t key;
    // blob points to the location where TList of Streamers starts
    // to free the acquired space, move blob pointer back by key_bytes: blob -= key.key_bytes and then free
    char *blob;
};

/**
 * Free Segmnets Record. For now a simple array impl.
 * TODO: does this come into play when we have more than single write/read multiple times, but also allow
 *   to modify the objects on disk -> for instance root allows to delete a record from disk
 */
struct free_segments_record_t {
    struct rkey_t key;
    int length;
    struct rfree_t *pfree;
};

struct llio_t {
    // owns
    uint64_t location;
    // owns
    struct rfile_context_t fctx;
    // owns
    struct rfile_header_t header;
    // owns
    struct top_dir_record_t top_dir_rec;
    // when reading -> read a blob and provide to the higher layer
    // when writing -> consume a blob and write to disk
    struct streamer_record_t streamer_record;
    struct free_segments_record_t free_segments_record;
};

//
// api: open/close
//
struct llio_t open_to_read(char const*filename);
struct llio_t open_to_write(char const*filename);
void close_from_read(struct llio_t*);
void close_from_write(struct llio_t*);

//
// private functions, modify the state
//
void read_file_header(struct llio_t*);
void read_streamer_record(struct llio_t*);
void read_free_segments_record(struct llio_t*);
void read_top_dir_record(struct llio_t*);

//
// public functions, no state modifications
//
struct directory_record_t read_dir_record_by_key(struct llio_t const*, struct rkey_t const*);
struct generic_record_t read_generic_record_by_location(struct llio_t const*, uint64_t);
struct keys_list_record_t read_keys_list_record_for_dir(struct llio_t const*, struct rdirectory_t const*);

/**
 * Writing logic step by step:
 * 1. write header
 * 2. write top directory record (given a name, we know the full size)
 * 3 -> N. Write Records
 * N+1. Write Top Dir Kyes List Record
 * N+2. ReWrite Top Directory Record
 * N+3. Write Streamer Record
 * N+4. Write Free Segment Record
 * N+5. Write End Byte
 * N+6. ReWrite flle header
 * 
 * Preconditions before writing a record:
 *  1. key and object blob are already properly generated
 *  2. key only misses the location to itself (position on file where it will be written)
 *
 * Postconditions after a record is written
 *  1. record is modified - key is updated to reflect the position in the file to write it to.
 *    Imporatnt for records that  depend on records that are written before them
 *  2. llio struct is modified to reflect the next position to write to in the file
 *  3. all the resources allocated within the function call are released
 *
 * Postconditions after a special record is written (streamer or free segments records):
 *  - for streamer -> modify the header struct to reflect the location of the streamer record in file
 *  - for free segments -> the same 
 */

// file header and top dir record do not increment the location
// as they are written twice, at open and second time at close
void write_file_header(struct llio_t*);
void write_top_dir_record(struct llio_t*);

/**
 * write a keys list for a directory
 * preconditions:
 *   - contents of the list itself are fully generated
 *   - the key of the key list record is generated, except for the location where to write the record.
 *      that includes the location of the folder that this keys list is assigned to. At the time of key generation,
 *      this location of the folder is already known, therefore must be assigned explicitly
 *
 * postconditions:
 *   - directory is updated to reflect the location where keys list is written
 *   - key of the keys list record is updated to reflect where it is written to
 */
void write_keys_list_record_for_dir(struct llio_t*, struct keys_list_record_t*, struct rdirectory_t*);

// do increment the location counter
void write_streamer_record(struct llio_t*);
void write_free_segments_record(struct llio_t*);
void write_end_byte(struct llio_t*);

// writing a generic record: key + blob
void write_generic_record(struct llio_t*, struct generic_record_t *);
void write_directory_record(struct llio_t*, struct directory_record_t*);

#endif // iolayer_h
