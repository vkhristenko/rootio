#ifndef bootstrap_h
#define bootstrap_h

#include <stdio.h>
#include <stdlib.h>

//
// file context
//
struct rfile_context_t {
    FILE *pfile;
};

/**
 * All of the constructor functions below are assumed to assign 
 * default values to the members of the struct or function parameters provide explicitly what
 * to assign to some of them
 */

/**
 * Assumptions about these simple strings:
 *   1. whenever struct rstring_t* is passed to a ctor of some type,
 *      it is expected that it takes ownership of it. Meaning can pass w/o memcopy.
 * TODO: our string implementation is flawed due to trying to free multiple times the same string
 */
struct rstring_t {
    int size;
    char *str;
};

void print_pstring(struct rstring_t const*);
void from_buf_pstring(char **buffer, struct rstring_t* pstring);
void to_buf_pstring(char **buffer, struct rstring_t const* pstring);
void ctor_pstring(struct rstring_t*);
void ctor_nomemcopy_pstring(struct rstring_t *ppstr, char *pstr, int size);
void ctor_memcopy_pstring(struct rstring_t *ppstr, char *pstr, int size);

/**
 * note: should only be called once when u own this string, as we do not enforce copying
 */
void dtor_pstring(struct rstring_t*);
uint32_t size_in_bytes_string(struct rstring_t const*);
uint32_t size_pstring(struct rstring_t const*); 

//
// Free Segment 
//
struct rfree_t {
    uint16_t version;
    uint64_t begin, end;
};

void print_pfree(struct rfree_t const*);
void from_buf_pfree(char **, struct rfree_t*);
void to_buf_pfree(char **, struct rfree_t const*);
uint32_t size_pfree(struct rfree_t const*);
void ctor_pfree(struct rfree_t*, uint64_t begin, uint64_t end);

// singly-linked list
struct free_node_t {
    struct rfree_t pfree;
    struct free_node_t* next;
};

void ctor_pfreenode(struct free_node_t* node);

//
// robject_t
//
struct robject_t {
    uint16_t version;
    uint32_t id;
    uint32_t bits;
};

uint32_t size_object(struct robject_t const*);
void from_buf_object(char ** buffer, struct robject_t *);
void to_buf_object(char ** buffer, struct robject_t const*);

//
// rdatime_t
//
struct rdatime_t {
    struct robject_t object;
    uint32_t raw;
};

void print_datime(struct rdatime_t  const*);
void from_buf_datime(char **buffer, struct rdatime_t *pdatime);
void to_buf_datime(char **buffer, struct rdatime_t const* pdatime);
void ctor_datime(struct rdatime_t *pdatime);
void ctor_fromval_datime(struct rdatime_t*, int32_t);
void dtor_datime(struct rdatime_t *pdatime);
uint32_t size_datime(struct rdatime_t const*);

//
// rnamed_t
//
struct rnamed_t {
    struct robject_t object;
    struct rstring_t name;
    struct rstring_t title;
};

void print_named(struct rnamed_t const *pnamed);
void from_buf_named(char **buffer, struct rnamed_t *pnamed);
void to_buf_named(char **buffer, struct rnamed_t const* pnamed);
void ctor_named(struct rnamed_t *pnamed);
void ctor_frompstring_named(struct rnamed_t*, struct rstring_t*, struct rstring_t*);
void dtor_named(struct rnamed_t *pnamed);
uint32_t size_named(struct rnamed_t const*);

//
// file header 
//
struct rfile_header_t {
    struct robject_t object;
    uint32_t version;
    uint32_t begin;
    uint64_t end;
    uint64_t seek_free;
    uint32_t nbytes_free;
    uint32_t nfree;
    uint32_t nbytes_name;
    char units;
    uint32_t compress;
    uint64_t seek_info;
    uint32_t nbytes_info;
};

void print_file_header(struct rfile_header_t const* pheader);
void ctor_file_header(struct rfile_header_t *pheader);
void dtor_file_header(struct rfile_header_t *pheader);
void from_buf_file_header(char **buffer, struct rfile_header_t *pheader);
void to_buf_file_header(char **buffer, struct rfile_header_t const* pheader);

//
// key product
//
struct rkey_t {
    struct robject_t object;
    uint32_t total_bytes;
    int32_t version;
    uint32_t obj_bytes;
    struct rdatime_t date_time;
    uint16_t key_bytes;
    uint16_t cycle;
    uint64_t seek_key;
    uint64_t seek_pdir;
    struct rstring_t class_name;
    struct rstring_t obj_name;
    struct rstring_t obj_title;
};

void print_key(struct rkey_t const* pkey);
void ctor_key(struct rkey_t *pkey);
void ctor_withnames_key(struct rkey_t *pkey, struct rstring_t* pclass_name, struct rstring_t *pobj_name, 
                        struct rstring_t *pobj_title);
void dtor_key(struct rkey_t *pkey);
void from_buf_key(char **buffer, struct rkey_t *pkey);
void to_buf_key(char **buffer, struct rkey_t const*);
uint32_t size_key(struct rkey_t const*);

struct key_list_t {
    int size;
    struct rkey_t *pkeys;
};

uint32_t size_keylist(struct key_list_t const*);

struct ruuid_t {
    uint32_t version;
    uint32_t time_low;
    uint16_t time_mid;
    uint16_t time_hi_and_version;
    uint8_t clock_seq_hi_and_reserved;
    uint8_t clock_seq_low;
    uint8_t node[6];
};

void print_uuid(struct ruuid_t const* uuid);
void ctor_uuid(struct ruuid_t *uuid);
void dtor_uuid(struct ruuid_t *);
void from_buf_uuid(char **buffer, struct ruuid_t *);
void to_buf_uuid(char **buffer, struct ruuid_t const*);
uint32_t size_uuid(struct ruuid_t const*);

//
// directory product
//
struct rdirectory_t {
    struct robject_t object;
    uint32_t version;
    struct rdatime_t date_time_c;
    struct rdatime_t date_time_m;
    uint32_t nbytes_keys;
    uint32_t nbytes_name;
    uint64_t seek_dir;
    uint64_t seek_parent;
    uint64_t seek_keys;
    struct ruuid_t uuid;
};

void ctor_dir(struct rdirectory_t *pdir);
void dtor_dir(struct rdirectory_t *pdir);
void print_dir(struct rdirectory_t const* pdir);
void from_buf_dir(char **buffer, struct rdirectory_t *pdir);
void to_buf_dir(char **buffer, struct rdirectory_t const* pdir);
uint32_t size_dir(struct rdirectory_t const*);

//
// Bootstrapping logic / functionality passing style
//
struct TopDirectory_v2 {
    struct rfile_header_t header;
    struct rkey_t key;
    struct rnamed_t named;
    struct rdirectory_t dir;
};

struct TopDirectory {
    struct rfile_header_t header;
    struct rdirectory_t dir;
};

struct rfile_context_t open_context(char const*, char const*);
void close_context(struct rfile_context_t);
void root_write(struct rfile_context_t, char const*, int);
void root_write_at_location(struct rfile_context_t, long, char const*, int);

/**
 * TODO: remove what is not used
 */
/*
void get_top_dir(struct rfile_context_t ctx, struct rdirectory_t *pdir);
void list_keys(struct rfile_context_t ctx, struct rdirectory_t *pdir, 
               struct rkey_t ** pkeys, int *pnkeys);
void get_blob(struct rfile_context_t ctx, struct rkey_t *pkey, char **blob);
void dump_contents(struct rfile_context_t ctx);

//
// Bootstrapping logic / functionality return style
//
struct TopDirectory read_top_dir(struct rfile_context_t);
struct TopDirectory_v2 read_top_dir_v2(struct rfile_context_t);
char* read_blob(struct rfile_context_t, struct rkey_t const*);
struct key_list_t read_keys(struct rfile_context_t, struct rdirectory_t const*);

void root_reserve_at_location(struct rfile_context_t, long location, int size);
void root_reserve(struct rfile_context_t, int size);
*/
#endif // bootstrap_h
