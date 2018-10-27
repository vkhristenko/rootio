#ifndef mops_core_iolayer_h
#define mops_core_iolayer_h

#include "rootio/core/logical_structure.h"

/**
 *
 * Local File System API
 *
 */
struct localfs_file_t {
    int fd;
};

struct localfs_file_context_t {
    struct localfs_file_t                       file;
    struct logical_structure_t                  structure;
};

struct localfs_file_t         localfs_open_file(char const*, int, int);
void                          localfs_close_file(struct localfs_file_t);
void                          localfs_write_to_file(struct localfs_file_t, char const*, int);
void                          localfs_write_to_file_at(struct localfs_file_t, long, char const*, int);
void                          localfs_seek_to(struct localfs_file_t, uint64_t);

//
// api: open/close
//
struct localfs_file_context_t localfs_open_to_read(char const *filename);
struct localfs_file_context_t localfs_open_to_write(char const *filename);
void                          localfs_close_from_read(struct localfs_file_context_t*);
void                          localfs_close_from_write(struct localfs_file_context_t*);

//
// api: private functions, modify the state
//
void                          localfs_read_file_header(struct localfs_file_context_t*);
void                          localfs_read_streamer_record(struct localfs_file_context_t*);
void                          localfs_read_free_segments_record(struct localfs_file_context_t*);
void                          localfs_read_top_dir_record(struct localfs_file_context_t*);

//
// public functions, no state modifications
//
struct directory_record_t     localfs_read_dir_record_by_key(struct localfs_file_context_t const*, struct key_t const*);
struct generic_record_t       localfs_read_generic_record_at(struct localfs_file_context_t const*, uint64_t);
struct keys_list_record_t     localfs_read_keys_list_record_for_dir(struct localfs_file_context_t const*, 
                                                                    struct directory_t const*);

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
void                          localfs_write_file_header(struct localfs_file_context_t*);
void                          localfs_write_top_dir_record(struct localfs_file_context_t*);

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
void                          localfs_write_keys_list_record_for_dir(struct localfs_file_context_t*, 
                                                                     struct keys_list_record_t*, 
                                                                     struct directory_t*);

// do increment the location counter
void                          localfs_write_streamer_record(struct localfs_file_context_t*);
void                          localfs_write_free_segments_record(struct localfs_file_context_t*);
void                          localfs_write_end_byte(struct localfs_file_context_t*);

// writing a generic record: key + blob
void                          localfs_write_generic_record(struct localfs_file_context_t*, 
                                                           struct generic_record_t *);
void                          localfs_write_directory_record(struct localfs_file_context_t*, 
                                                             struct directory_record_t*);

#endif // mops_core_iolayer_h
