#include "bootstrap.h"
#include "aux.h"
#include "debug.h"

//
// PDatime
//
void from_buf_datime(char **buffer, struct PDatime *pdatime) {
    pdatime->raw = get_u32(buffer);
}

void to_buf_datime(char **buffer, struct PDatime *pdatime) {
    put_u32(buffer, pdatime->raw);
}

void ctor_datime(struct PDatime *pdatime) {}

void dtor_datime(struct PDatime *pdatime) {}

void print_datime(struct PDatime *pdatime) {
    printf("Product <PDatime>:\n");
    printf("-------------------\n");
    print_u32(pdatime->raw);
}

//
// PNamed
//
void print_named(struct PNamed *pnamed) {
    printf("Product <PNamed>:\n");
    printf("-------------------\n");
    print_string(pnamed->name);
    print_string(pnamed->title);
}

void from_buf_named(char **buffer, struct PNamed *pnamed) {
    get_string(buffer, &pnamed->name);
    get_string(buffer, &pnamed->title);
}

void to_buf_named(char **buffer, struct PNamed *pnamed) {
    put_string(buffer, pnamed->name);
    put_string(buffer, pnamed->title);
}

void ctor_named(struct PNamed *pnamed) {}

void dtor_named(struct PNamed *pnamed) {
    free(pnamed->name);
    free(pnamed->title);
}

//
// file header 
//
void print_file_header(struct PFileHeader *pheader) {
    printf("Product <PFileHeader>:\n");
    printf("-----------------------\n");
    print_u32(pheader->version);
    print_u32(pheader->begin);
    print_u64(pheader->end);
    print_u64(pheader->seek_free);
    print_u32(pheader->nbytes_free);
    print_u32(pheader->nfree);
    print_u32(pheader->nbytes_name);
    print_char(pheader->units);
    print_u32(pheader->compress);
    print_u64(pheader->seek_info);
    print_u32(pheader->nbytes_info);
}

void ctor_file_header(struct PFileHeader *pheader) {}

void dtor_file_header(struct PFileHeader *pheader) {}

void from_buf_file_header(char **buffer, struct PFileHeader *pheader) {
    if (strncmp(*buffer, "root", 4)) {
        printf("not a root file\n");
        assert(0);
    }
    *buffer+=4;
    
    pheader->version = get_u32(buffer);
    pheader->begin = get_u32(buffer);
    int is_large_file = pheader->version > 1000000u;
    if (is_large_file) {
        pheader->end = get_u64(buffer);
        pheader->seek_free = get_u64(buffer);
        pheader->nbytes_free = get_u32(buffer);
        pheader->nfree = get_u32(buffer);
        pheader->nbytes_name = get_u32(buffer);
        pheader->units = **buffer; (*buffer)++; // manually change the buffer pointer
        pheader->compress = get_u32(buffer);
        pheader->seek_info = get_u64(buffer);
        pheader->nbytes_info = get_u32(buffer);
    } else {
        pheader->end = get_u32(buffer);
        pheader->seek_free = get_u32(buffer);
        pheader->nbytes_free = get_u32(buffer);
        pheader->nfree = get_u32(buffer);
        pheader->nbytes_name = get_u32(buffer);
        pheader->units = **buffer; (*buffer)++; // manually change the buffer pointer
        pheader->compress = get_u32(buffer);
        pheader->seek_info = get_u32(buffer);
        pheader->nbytes_info = get_u32(buffer);
    }
}

void to_buf_file_header(char **buffer, struct PFileHeader *pheader) {
    put_u32(buffer, pheader->version);
    put_u32(buffer, pheader->begin);
    int is_large_file = pheader->version > 1000000u;
    if (is_large_file) {
        put_u64(buffer, pheader->end);
        put_u64(buffer, pheader->seek_free);
        put_u32(buffer, pheader->nbytes_free);
        put_u32(buffer, pheader->nfree);
        put_u32(buffer, pheader->nbytes_name);
        **buffer = pheader->units; (*buffer)++;
        put_u32(buffer, pheader->compress);
        put_u64(buffer, pheader->seek_info);
        put_u32(buffer, pheader->nbytes_info);
    } else {
        put_u32(buffer, (uint32_t)pheader->end);
        put_u32(buffer, (uint32_t)pheader->seek_free);
        put_u32(buffer, pheader->nbytes_free);
        put_u32(buffer, pheader->nfree);
        put_u32(buffer, pheader->nbytes_name);
        **buffer = pheader->units; (*buffer)++;
        put_u32(buffer, pheader->compress);
        put_u32(buffer, (uint32_t)pheader->seek_info);
        put_u32(buffer, pheader->nbytes_info);
    }
}

//
// key product
//
void print_key(struct PKey *pkey) {
    printf("Product <PKey>:\n");
    printf("----------------\n");
    print_u32(pkey->total_bytes);
    print_u32(pkey->version);
    print_u32(pkey->obj_bytes);
    print_datime(&(pkey->date_time));
    print_u16(pkey->key_bytes);
    print_u16(pkey->cycle);
    print_u64(pkey->seek_key);
    print_u64(pkey->seek_pdir);
    print_string(pkey->class_name);
    print_string(pkey->obj_name);
    print_string(pkey->obj_title);
}

void ctor_key(struct PKey *pkey) {}

void dtor_key(struct PKey *pkey) {
    free(pkey->class_name);
    free(pkey->obj_name);
    free(pkey->obj_title);
}

void from_buf_key(char **buffer, struct PKey *pkey) {
    pkey->total_bytes = get_u32(buffer);
    pkey->version = get_version(buffer);
    pkey->obj_bytes = get_u32(buffer);
    from_buf_datime(buffer, &(pkey->date_time));
    pkey->key_bytes = get_u16(buffer);
    pkey->cycle = get_u16(buffer);
    if (pkey->version > 1000) {
        pkey->seek_key = get_u64(buffer);
        pkey->seek_pdir = get_u64(buffer);
    } else {
        pkey->seek_key = get_u32(buffer);
        pkey->seek_pdir = get_u32(buffer);
    }
    get_string(buffer, &(pkey->class_name));
    get_string(buffer, &(pkey->obj_name));
    get_string(buffer, &(pkey->obj_title));
}

void to_buf_key(char **buffer, struct PKey *pkey) {
    put_u32(buffer, pkey->total_bytes);
    put_version(buffer, pkey->version);
    put_u32(buffer, pkey->obj_bytes);
    to_buf_datime(buffer, &(pkey->date_time));
    put_u16(buffer, pkey->key_bytes);
    put_u16(buffer, pkey->cycle);
    if (pkey->version > 1000) {
        put_u64(buffer, pkey->seek_key);
        put_u64(buffer, pkey->seek_pdir);
    } else {
        put_u32(buffer, (uint32_t)pkey->seek_key);
        put_u32(buffer, (uint32_t)pkey->seek_pdir);
    }

    put_string(buffer, pkey->class_name);
    put_string(buffer, pkey->obj_name);
    put_string(buffer, pkey->obj_title);
}

//
// directory product
//
void ctor_dir(struct PDirectory *pdir) {}

void dtor_dir(struct PDirectory *pdir) {}

void print_dir(struct PDirectory *pdir) {
    printf("Product <PDirectory>:\n");
    printf("-----------------------\n");
    print_u32(pdir->version);
    print_datime(&(pdir->date_time_c));
    print_datime(&(pdir->date_time_m));
    print_u32(pdir->nbytes_keys);
    print_u32(pdir->nbytes_name);
    print_u64(pdir->seek_dir);
    print_u64(pdir->seek_parent);
    print_u64(pdir->seek_keys);
}

void from_buf_dir(char **buffer, struct PDirectory *pdir) {
    pdir->version = get_version(buffer);
    from_buf_datime(buffer, &(pdir->date_time_c)); // ptr is taken care of
    from_buf_datime(buffer, &(pdir->date_time_m)); // same
    pdir->nbytes_keys = get_u32(buffer);
    pdir->nbytes_name = get_u32(buffer);
    if (pdir->version > 1000) {
        pdir->seek_dir = get_u64(buffer);
        pdir->seek_parent = get_u64(buffer);
        pdir->seek_keys = get_u64(buffer);
    } else {
        pdir->seek_dir = get_u32(buffer);
        pdir->seek_parent = get_u32(buffer);
        pdir->seek_keys = get_u32(buffer);
    }
}

void to_buf_dir(char **buffer, struct PDirectory *pdir) {
    put_version(buffer, pdir->version);
    to_buf_datime(buffer, &(pdir->date_time_c));
    to_buf_datime(buffer, &(pdir->date_time_m));
    put_u32(buffer, pdir->nbytes_keys);
    put_u32(buffer, pdir->nbytes_name);
    if (pdir->version > 1000) {
        put_u64(buffer, pdir->seek_dir);
        put_u64(buffer, pdir->seek_parent);
        put_u64(buffer, pdir->seek_keys);
    } else {
        put_u32(buffer, (uint32_t)pdir->seek_dir);
        put_u32(buffer, (uint32_t)pdir->seek_parent);
        put_u32(buffer, (uint32_t)pdir->seek_keys);
    }
}

void list_keys(struct FileContext ctx, struct PDirectory *pdir, 
               struct PKey ** pkeys, int *pnkeys) {
    // read into the buffer
    char *buffer = malloc(pdir->nbytes_keys);
    fseek(ctx.pfile, pdir->seek_keys, SEEK_SET);
    size_t nbytes = fread((void*)buffer, 1, pdir->nbytes_keys, ctx.pfile);

    // tlist key
    struct PKey key;
    ctor_key(&key);
    from_buf_key(&buffer, &key);
    print_key(&key);
    printf("\n");

    
    // get how many keys we have and allocate enough space on heap
    *pnkeys = get_u32(&buffer);
    *pkeys = malloc(sizeof(struct PKey)*(*pnkeys));

    // iterate, construct, cast from from the in-memory buffer
    for (int i=0; i<(*pnkeys); i++) {
        // construct the i-th key in the allocate space
        ctor_key(&((*pkeys)[i]));
        // cast 
        from_buf_key(&buffer, &((*pkeys)[i]));
    }
}

//
// some logic
//
void get_top_dir(struct FileContext ctx, struct PDirectory *pdir) {
    char *buffer = malloc(300);
    size_t nbytes = fread((void*)buffer, 1, 300, ctx.pfile);
    char *start = buffer;

    printf("read nbytes = %lu\n", nbytes);
    dump_raw(buffer, nbytes, 20);
    dump_raw(buffer, nbytes, 20);
    dump_raw(buffer, nbytes, 20);

    // get the file header
    struct PFileHeader header;
    ctor_file_header(&header);
    from_buf_file_header(&buffer, &header);
    print_file_header(&header);

    printf("\n");

    // top dir key
    start+=100;
    struct PKey key;
    ctor_key(&key);
    from_buf_key(&start, &key);
    print_key(&key);
    printf("\n");

    // get named stuff
    struct PNamed named;
    ctor_named(&named);
    from_buf_named(&start, &named);
    print_named(&named);
    printf("\n");

    // dir
    from_buf_dir(&start, pdir);
    print_dir(pdir);
    printf("\n");
}

void get_blob(struct FileContext ctx, struct PKey *pkey, char **blob) {
    // read in the pre-allocated blob
    fseek(ctx.pfile, pkey->seek_key, SEEK_SET);
    size_t nbytes = fread((void*)*blob, 1, pkey->total_bytes, ctx.pfile);

    struct PKey key;
    ctor_key(&key);
    from_buf_key(blob, &key);
}

void dump_contents(struct FileContext ctx) {
    // get top dir
    struct PDirectory dir;
    ctor_dir(&dir);
    get_top_dir(ctx, &dir);

    // list keys in this dir
    struct PKey *pkeys; 
    int nkeys;
    list_keys(ctx, &dir, &pkeys, &nkeys);
    for (int i=0; i<nkeys; i++) {
        print_key(&(pkeys[i]));
        printf("\n");
    }
}

struct FileContext open_context(char* filename, char *opts) {
    struct FileContext ctx;
    ctx.pfile = fopen(filename, opts);
    return ctx;
}  

void close_context(struct FileContext ctx) {
    fclose(ctx.pfile);
}

struct TopDirectory read_top_dir(struct FileContext ctx) {
    struct TopDirectory root;

    // initial buffer of 300bytes is a safe assumption
    char *buffer = malloc(300);
    size_t nbytes = fread((void*)buffer, 1, 300, ctx.pfile);
    char *start = buffer;

    // get the file header
    ctor_file_header(&root.header);
    from_buf_file_header(&buffer, &root.header);

    // top dir key
    start+=100;
    struct PKey key;
    ctor_key(&key);
    from_buf_key(&start, &key);

    // get named stuff
    struct PNamed named;
    ctor_named(&named);
    from_buf_named(&start, &named);

    // dir
    from_buf_dir(&start, &root.dir);

    return root;
}

char* read_blob(struct FileContext ctx, struct PKey const *pkey) {
    char *blob = malloc(pkey->total_bytes);

    // read in the allocated blob
    fseek(ctx.pfile, pkey->seek_key, SEEK_SET);
    size_t nbytes = fread(blob, 1, pkey->total_bytes, ctx.pfile);

    return blob;
}

struct KeyList read_keys(struct FileContext ctx, struct PDirectory const* pdir) {
    struct KeyList klist;

    // read into the buffer
    char *buffer = malloc(pdir->nbytes_keys);
    fseek(ctx.pfile, pdir->seek_keys, SEEK_SET);
    size_t nbytes = fread((void*)buffer, 1, pdir->nbytes_keys, ctx.pfile);

    // tlist key
    struct PKey key;
    ctor_key(&key);
    from_buf_key(&buffer, &key);
    
    // get how many keys we have and allocate enough space on heap
    klist.size = get_u32(&buffer);
    klist.pkeys = malloc(sizeof(struct PKey)*(klist.size));

    // iterate, construct, cast from from the in-memory buffer
    for (int i=0; i<(klist.size); i++) {
        // construct the i-th key in the allocate space
        ctor_key(&((klist.pkeys)[i]));
        // cast 
        from_buf_key(&buffer, &((klist.pkeys)[i]));
    }

    return klist;
}

void root_reserve(struct FileContext ctx, int size) {
    char *buf = malloc(size);
    fwrite((void*)buf, 1, size, ctx.pfile);
}

void root_reserve_at_location(struct FileContext ctx, long location, int size) {
    fseek(ctx.pfile, location, SEEK_SET);

    char *buf = malloc(size);
    fwrite((void*)buf, 1, size, ctx.pfile);
}

void root_write(struct FileContext ctx, char *buf, int size) { 
    fwrite((void*)buf, 1, size, ctx.pfile);
}

void root_write_at_location(struct FileContext ctx, long location, char *buf, int size) {
    fseek(ctx.pfile, location, SEEK_SET);
    fwrite((void*)buf, 1, size, ctx.pfile);
}
