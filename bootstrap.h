#ifndef bootstrap_h
#define bootstrap_h

#include <stdio.h>
#include <stdlib.h>

//
// file context
//
struct FileContext {
    FILE *pfile;
};

/**
 * All of the constructor functions below are assumed to assign 
 * default values to the members of the struct or function parameters provide explicitly what
 * to assign to some of them
 */

/**
 * Assumptions about these simple strings:
 *   1. whenever struct PString* is passed to a ctor of some type,
 *      it is expected that it takes ownership of it. Meaning can pass w/o memcopy.
 * TODO: our string implementation is flawed due to trying to free multiple times the same string
 */
struct PString {
    int size;
    char *str;
};

void print_pstring(struct PString*);
void from_buf_pstring(char **buffer, struct PString* pstring);
void to_buf_pstring(char **buffer, struct PString const* pstring);
void ctor_pstring(struct PString*);
void ctor_nomemcopy_pstring(struct PString *ppstr, char *pstr, int size);
void ctor_memcopy_pstring(struct PString *ppstr, char *pstr, int size);

/**
 * note: should only be called once when u own this string, as we do not enforce copying
 */
void dtor_pstring(struct PString*);
uint32_t size_in_bytes_string(struct PString *);
uint32_t size_pstring(struct PString *); 

//
// Free Segment 
//
struct PFree {
    uint16_t version;
    uint64_t begin, end;
};

void print_pfree(struct PFree*);
void from_buf_pfree(char **, struct PFree*);
void to_buf_pfree(char **, struct PFree*);
uint32_t size_pfree(struct PFree*);
void ctor_pfree(struct PFree*, uint64_t begin, uint64_t end);

// singly-linked list
struct PFreeNode {
    struct PFree pfree;
    struct PFreeNode* next;
};

void ctor_pfreenode(struct PFreeNode* node);

//
// PObject
//
struct PObject {
    uint16_t version;
    uint32_t id;
    uint32_t bits;
};

uint32_t size_object(struct PObject*);
void from_buf_object(char ** buffer, struct PObject *);
void to_buf_object(char ** buffer, struct PObject*);

//
// PDatime
//
struct PDatime {
    struct PObject object;
    uint32_t raw;
};

void print_datime(struct PDatime *pdatime);
void from_buf_datime(char **buffer, struct PDatime *pdatime);
void to_buf_datime(char **buffer, struct PDatime *pdatime);
void ctor_datime(struct PDatime *pdatime);
void ctor_fromval_datime(struct PDatime*, int32_t);
void dtor_datime(struct PDatime *pdatime);
uint32_t size_datime(struct PDatime*);

//
// PNamed
//
struct PNamed {
    struct PObject object;
    struct PString name;
    struct PString title;
};

void print_named(struct PNamed *pnamed);
void from_buf_named(char **buffer, struct PNamed *pnamed);
void to_buf_named(char **buffer, struct PNamed *pnamed);
void ctor_named(struct PNamed *pnamed);
void ctor_frompstring_named(struct PNamed*, struct PString*, struct PString*);
void dtor_named(struct PNamed *pnamed);
uint32_t size_named(struct PNamed*);

//
// file header 
//
struct PFileHeader {
    struct PObject object;
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

void print_file_header(struct PFileHeader *pheader);
void ctor_file_header(struct PFileHeader *pheader);
void dtor_file_header(struct PFileHeader *pheader);
void from_buf_file_header(char **buffer, struct PFileHeader *pheader);
void to_buf_file_header(char **buffer, struct PFileHeader *pheader);

//
// key product
//
struct PKey {
    struct PObject object;
    uint32_t total_bytes;
    int32_t version;
    uint32_t obj_bytes;
    struct PDatime date_time;
    uint16_t key_bytes;
    uint16_t cycle;
    uint64_t seek_key;
    uint64_t seek_pdir;
    struct PString class_name;
    struct PString obj_name;
    struct PString obj_title;
};

void print_key(struct PKey *pkey);
void ctor_key(struct PKey *pkey);
void ctor_withnames_key(struct PKey *pkey, struct PString* pclass_name, struct PString *pobj_name, 
                        struct PString *pobj_title);
void dtor_key(struct PKey *pkey);
void from_buf_key(char **buffer, struct PKey *pkey);
void to_buf_key(char **buffer, struct PKey*);
uint32_t size_key(struct PKey*);

struct KeyList {
    int size;
    struct PKey *pkeys;
};

uint32_t size_keylist(struct KeyList*);

struct PUUID {
    uint16_t version;
    uint32_t time_low;
    uint16_t time_mid;
    uint16_t time_hi_and_version;
    uint8_t clock_seq_hi_and_reserved;
    uint8_t clock_seq_low;
    uint8_t node[6];
};

void print_uuid(struct PUUID *uuid);
void ctor_uuid(struct PUUID *uuid);
void dtor_uuid(struct PUUID *);
void from_buf_uuid(char **buffer, struct PUUID *);
void to_buf_uuid(char **buffer, struct PUUID *);
uint32_t size_uuid(struct PUUID*);

//
// directory product
//
struct PDirectory {
    struct PObject object;
    uint32_t version;
    struct PDatime date_time_c;
    struct PDatime date_time_m;
    uint32_t nbytes_keys;
    uint32_t nbytes_name;
    uint64_t seek_dir;
    uint64_t seek_parent;
    uint64_t seek_keys;
    struct PUUID uuid;
};

void ctor_dir(struct PDirectory *pdir);
void dtor_dir(struct PDirectory *pdir);
void print_dir(struct PDirectory *pdir);
void from_buf_dir(char **buffer, struct PDirectory *pdir);
void to_buf_dir(char **buffer, struct PDirectory *pdir);
uint32_t size_dir(struct PDirectory*);

//
// Bootstrapping logic / functionality passing style
//
struct TopDirectory_v2 {
    struct PFileHeader header;
    struct PKey key;
    struct PNamed named;
    struct PDirectory dir;
};

struct TopDirectory {
    struct PFileHeader header;
    struct PDirectory dir;
};

void get_top_dir(struct FileContext ctx, struct PDirectory *pdir);
void list_keys(struct FileContext ctx, struct PDirectory *pdir, 
               struct PKey ** pkeys, int *pnkeys);
void get_blob(struct FileContext ctx, struct PKey *pkey, char **blob);
void dump_contents(struct FileContext ctx);

struct FileContext open_context(char*, char*);
void close_context(struct FileContext);

//
// Bootstrapping logic / functionality return style
//
struct TopDirectory read_top_dir(struct FileContext);
struct TopDirectory_v2 read_top_dir_v2(struct FileContext);
char* read_blob(struct FileContext, struct PKey const*);
struct KeyList read_keys(struct FileContext, struct PDirectory const*);

void root_reserve_at_location(struct FileContext, long location, int size);
void root_reserve(struct FileContext, int size);
void root_write(struct FileContext, char*, int);
void root_write_at_location(struct FileContext, long, char*, int);

#endif // bootstrap_h
