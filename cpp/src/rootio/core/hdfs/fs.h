#ifndef rootio_core_hdfs_fs_h
#define rootio_core_hdfs_fs_h

#include "hdfs.h"
#include "rootio/core/logical_structure.h"

/*
 * Hadoop File System API
 */
struct hdfs_file_t {
    hdfsFile                            fd;
    hdfsFS                              fs;
};

struct hdfs_file_context_t {
    struct hdfs_file_t                  file;
    struct logical_structure_t           structure;
};

/*
 * file api
 */
struct hdfs_file_t            hdfs_open_file(char const*, int);
void                          hdfs_close_file(struct hdfs_file_t);
void                          hdfs_write_to_file(struct hdfs_file_t,
                                    char const*, int);
void                          hdfs_write_to_file_at(struct hdfs_file_t,
                                    long, char const*, int);
uint64_t                      hdfs_read_from_file(struct hdfs_file_t,
                                    char *buffer, uint64_t);
void                          hdfs_seek_to(struct hdfs_file_t, uint64_t);

/*
 * context api
 */
struct hdfs_file_context_t    hdfs_open_to_read(char const *filename);
struct hdfs_file_context_t    hdfs_open_to_write(char const *filename);
void                          hdfs_close_from_read(struct hdfs_file_context_t*);
void                          hdfs_close_from_write(
                                    struct hdfs_file_context_t*);

//
// api: private functions, modify the state
//
void                          hdfs_read_file_header(
                                    struct hdfs_file_context_t*);
void                          hdfs_read_top_dir_record(
                                    struct hdfs_file_context_t*);

//
// public functions, no state modifications
//
struct streamer_record_t      hdfs_read_streamer_record(struct 
                                    hdfs_file_context_t*);
struct free_segments_record_t hdfs_read_free_segments_record(struct 
                                    hdfs_file_context_t*);
struct directory_record_t     hdfs_read_dir_record_by_key(
                                    struct hdfs_file_context_t const*, 
                                    struct key_t const*);
struct generic_record_t       hdfs_read_generic_record_at(
                                    struct hdfs_file_context_t const*,
                                    uint64_t);
struct keys_list_record_t     hdfs_read_keys_list_record_for_dir(
                                    struct hdfs_file_context_t const*,
                                    struct directory_t const*);

void                          hdfs_write_file_header(
                                    struct hdfs_file_context_t*);
void                          hdfs_write_top_dir_record(
                                    struct hdfs_file_context_t*);

void                          hdfs_write_keys_list_record_for_dir(
                                    struct hdfs_file_context_t*,
                                    struct keys_list_record_t*,
                                    struct directory_t*);

// do increment the location counter
void                          hdfs_write_streamer_record(
                                    struct hdfs_file_context_t*,
                                    struct streamer_record_t*);
void                          hdfs_write_free_segments_record(
                                    struct hdfs_file_context_t*,
                                    struct free_segments_record_t*);
void                          hdfs_write_end_byte(
                                    struct hdfs_file_context_t*);
// writing a generic record: key + blob
void                          hdfs_write_generic_record(
                                    struct hdfs_file_context_t*,
                                    struct generic_record_t *);
void                          hdfs_write_directory_record(
                                    struct hdfs_file_context_t*,
                                    struct directory_record_t*);

#endif // rootio_core_hdfs_fs_h
