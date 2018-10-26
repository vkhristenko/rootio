#ifndef bootstrap_h
#define bootstrap_h

#include <stdio.h>
#include <stdlib.h>

//
// file context
//
struct file_context_t {
    int fd;
};

/**
 * All of the constructor functions below are assumed to assign 
 * default values to the members of the struct or function parameters provide explicitly what
 * to assign to some of them
 */

/**
 * Assumptions about these simple strings:
 *   1. whenever struct string_t* is passed to a ctor of some type,
 *      it is expected that it takes ownership of it. Meaning can pass w/o memcopy.
 * TODO: our string implementation is flawed due to trying to free multiple times the same string
 */
struct string_t {
    int size;
    char *str;
};

void print_pstring(struct string_t const*);
void from_buf_pstring(char **buffer, struct string_t* pstring);
void to_buf_pstring(char **buffer, struct string_t const* pstring);
void ctor_pstring(struct string_t*);
void ctor_nomemcopy_pstring(struct string_t *ppstr, char *pstr, int size);
void ctor_memcopy_pstring(struct string_t *ppstr, char *pstr, int size);

/**
 * note: should only be called once when u own this string, as we do not enforce copying
 */
void dtor_pstring(struct string_t*);
uint32_t size_in_bytes_string(struct string_t const*);
uint32_t size_pstring(struct string_t const*); 

//
// Free Segment 
//
struct free_t {
    uint16_t version;
    uint64_t begin, end;
};

void print_pfree(struct free_t const*);
void from_buf_pfree(char **, struct free_t*);
void to_buf_pfree(char **, struct free_t const*);
uint32_t size_pfree(struct free_t const*);
void ctor_pfree(struct free_t*, uint64_t begin, uint64_t end);

// singly-linked list
struct free_node_t {
    struct free_t pfree;
    struct free_node_t* next;
};

void ctor_pfreenode(struct free_node_t* node);

//
// object_t
//
struct object_t {
    uint16_t version;
    uint32_t id;
    uint32_t bits;
};

uint32_t size_object(struct object_t const*);
void from_buf_object(char ** buffer, struct object_t *);
void to_buf_object(char ** buffer, struct object_t const*);

//
// datime_t
//
struct datime_t {
    struct object_t object;
    uint32_t raw;
};

void print_datime(struct datime_t  const*);
void from_buf_datime(char **buffer, struct datime_t *pdatime);
void to_buf_datime(char **buffer, struct datime_t const* pdatime);
void ctor_datime(struct datime_t *pdatime);
void ctor_fromval_datime(struct datime_t*, int32_t);
void dtor_datime(struct datime_t *pdatime);
uint32_t size_datime(struct datime_t const*);

//
// named_t
//
struct named_t {
    struct object_t object;
    struct string_t name;
    struct string_t title;
};

void print_named(struct named_t const *pnamed);
void from_buf_named(char **buffer, struct named_t *pnamed);
void to_buf_named(char **buffer, struct named_t const* pnamed);
void ctor_named(struct named_t *pnamed);
void ctor_frompstring_named(struct named_t*, struct string_t*, struct string_t*);
void dtor_named(struct named_t *pnamed);
uint32_t size_named(struct named_t const*);

//
// file header 
//
struct file_header_t {
    struct object_t object;
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

void print_file_header(struct file_header_t const* pheader);
void ctor_file_header(struct file_header_t *pheader);
void dtor_file_header(struct file_header_t *pheader);
void from_buf_file_header(char **buffer, struct file_header_t *pheader);
void to_buf_file_header(char **buffer, struct file_header_t const* pheader);

//
// key product
//
struct key_t {
    struct object_t object;
    uint32_t total_bytes;
    int32_t version;
    uint32_t obj_bytes;
    struct datime_t date_time;
    uint16_t key_bytes;
    uint16_t cycle;
    uint64_t seek_key;
    uint64_t seek_pdir;
    struct string_t class_name;
    struct string_t obj_name;
    struct string_t obj_title;
};

void print_key(struct key_t const* pkey);
void ctor_key(struct key_t *pkey);
void ctor_withnames_key(struct key_t *pkey, struct string_t* pclass_name, struct string_t *pobj_name, 
                        struct string_t *pobj_title);
void dtor_key(struct key_t *pkey);
void from_buf_key(char **buffer, struct key_t *pkey);
void to_buf_key(char **buffer, struct key_t const*);
uint32_t size_key(struct key_t const*);

struct key_list_t {
    int size;
    struct key_t *pkeys;
};

uint32_t size_keylist(struct key_list_t const*);

struct uuid_t {
    uint32_t version;
    uint32_t time_low;
    uint16_t time_mid;
    uint16_t time_hi_and_version;
    uint8_t clock_seq_hi_and_reserved;
    uint8_t clock_seq_low;
    uint8_t node[6];
};

void print_uuid(struct uuid_t const* uuid);
void ctor_uuid(struct uuid_t *uuid);
void dtor_uuid(struct uuid_t *);
void from_buf_uuid(char **buffer, struct uuid_t *);
void to_buf_uuid(char **buffer, struct uuid_t const*);
uint32_t size_uuid(struct uuid_t const*);

//
// directory product
//
struct directory_t {
    struct object_t object;
    uint32_t version;
    struct datime_t date_time_c;
    struct datime_t date_time_m;
    uint32_t nbytes_keys;
    uint32_t nbytes_name;
    uint64_t seek_dir;
    uint64_t seek_parent;
    uint64_t seek_keys;
    struct uuid_t uuid;
};

void ctor_dir(struct directory_t *pdir);
void dtor_dir(struct directory_t *pdir);
void print_dir(struct directory_t const* pdir);
void from_buf_dir(char **buffer, struct directory_t *pdir);
void to_buf_dir(char **buffer, struct directory_t const* pdir);
uint32_t size_dir(struct directory_t const*);

//
// Bootstrapping logic / functionality passing style
//
struct TopDirectory_v2 {
    struct file_header_t header;
    struct key_t key;
    struct named_t named;
    struct directory_t dir;
};

struct TopDirectory {
    struct file_header_t header;
    struct directory_t dir;
};

struct file_context_t open_context(char const*, int);
void close_context(struct file_context_t);
void root_write(struct file_context_t, char const*, int);
void root_write_at_location(struct file_context_t, long, char const*, int);
void root_seek(struct file_context_t, uint64_t);

#endif // bootstrap_h
