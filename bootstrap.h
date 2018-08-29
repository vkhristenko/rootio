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

struct PString {
    int size;
    char *str;
};

void print_pstring(struct PString*);
void from_buf_pstring(char **buffer, struct PString* pstring);
void to_buf_pstring(char **buffer, struct PString const* pstring);
void ctor_pstring(struct PString*);
void dtor_pstring(struct PString*);

//
// PObject
//
struct PObject {};

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
void dtor_datime(struct PDatime *pdatime);

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
void dtor_named(struct PNamed *pnamed);

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
void dtor_key(struct PKey *pkey);
void from_buf_key(char **buffer, struct PKey *pkey);
void to_buf_key(char **buffer, struct PKey*);

struct KeyList {
    int size;
    struct PKey *pkeys;
};

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
};

void ctor_dir(struct PDirectory *pdir);
void dtor_dir(struct PDirectory *pdir);
void print_dir(struct PDirectory *pdir);
void from_buf_dir(char **buffer, struct PDirectory *pdir);
void to_buf_dir(char **buffer, struct PDirectory *pdir);

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

//
// Bootstrapping logic / functionality passing style
//
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
