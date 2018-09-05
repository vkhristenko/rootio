#include "bootstrap.h"
#include "aux.h"
#include "debug.h"

//
// POJect Product
//
void from_buf_object(char ** buffer, struct PObject *obj) {
    obj->version = get_version(buffer);
    obj->id = get_u32(buffer);
    obj->bits = get_u32(buffer);
}

void to_buf_object(char ** buffer, struct PObject *obj) {
    put_version(buffer, obj->version);
    put_u32(buffer, obj->id);
    put_u32(buffer, obj->bits);
}

uint32_t size_object(struct PObject* obj) {
    return 10;
}

//
// pstring
//
void print_pstring(struct PString* ppstr) {
    printf("Product <PString:>\n");
    printf("--------------------\n");
    printf("size = %d\n", ppstr->size);
    print_string(ppstr->str);
}

void from_buf_pstring(char **buffer, struct PString *ppstr) {
    ppstr->size = get_string(buffer, &ppstr->str);
}

void to_buf_pstring(char **buffer, struct PString const* ppstr) {
    put_string(buffer, ppstr->str, ppstr->size);
}

void ctor_pstring(struct PString *ppstr) {
    ppstr->str = "";
    ppstr->size = 0;
}

void ctor_nomemcopy_pstring(struct PString *ppstr, char *pstr, int size) {
    ppstr->size = size;
    ppstr->str = pstr;
}

void ctor_memcopy_pstring(struct PString *ppstr, char *pstr, int size) {
    ppstr->size = size;
    ppstr->str = malloc(ppstr->size);
    memcpy(ppstr->str, pstr, ppstr->size);
}

void dtor_pstring(struct PString *ppstr) {
    free(ppstr->str);
}

uint32_t size_in_bytes_string(struct PString *pstr) {
    uint32_t nbytes = 1;
    if (pstr->size > 0 && pstr->size < 255)
        nbytes += pstr->size;
    else if (pstr->size >= 255)
        nbytes += 3 + pstr->size;

    return nbytes;
}

uint32_t size_pstring(struct PString* pstr) {
    return (uint32_t)(pstr->size);
}

void print_pfree(struct PFree* pfree) {
    print_u16(pfree->version);
    print_u64(pfree->begin);
    print_u64(pfree->end);
}

void from_buf_pfree(char **buffer, struct PFree *pfree) {
    pfree->version = get_version(buffer);
    if (pfree->version > 1000) {
        pfree->begin = get_u64(buffer);
        pfree->end = get_u64(buffer);
    } else {
        pfree->begin = get_u32(buffer);
        pfree->end = get_u32(buffer);
    }
}

void to_buf_pfree(char **buffer, struct PFree *pfree) {
    put_version(buffer, pfree->version);
    if (pfree->version > 1000) {
        put_u64(buffer, pfree->begin);
        put_u64(buffer, pfree->end);
    } else {
        put_u32(buffer, (uint32_t)pfree->begin);
        put_u32(buffer, (uint32_t)pfree->end);
    }
}

uint32_t size_pfree(struct PFree *pfree) {
    int nbytes = 10;
    if (pfree->version > 1000)
        nbytes += 8;
    return nbytes;
}

void ctor_pfree(struct PFree *pfree, uint64_t begin, uint64_t end) {
    pfree->version = 1;
    pfree->begin = begin;
    pfree->end = end;
}

void ctor_pfreenode(struct PFreeNode *node) {
    node->next = NULL;
}

//
// PDatime
//
void from_buf_datime(char **buffer, struct PDatime *pdatime) {
    pdatime->raw = get_u32(buffer);
}

void to_buf_datime(char **buffer, struct PDatime *pdatime) {
    put_u32(buffer, pdatime->raw);
}

void ctor_datime(struct PDatime *pdatime) {
    pdatime->raw = 12345;
}

void dtor_datime(struct PDatime *pdatime) {}

void print_datime(struct PDatime *pdatime) {
    printf("Product <PDatime>:\n");
    printf("-------------------\n");
    print_u32(pdatime->raw);
}

uint32_t size_datime(struct PDatime *pdatime) {
    return 4;
}

void ctor_fromval_datime(struct PDatime* pdatime, int32_t value) {
    pdatime->raw = value;
}

//
// PNamed
//
void print_named(struct PNamed *pnamed) {
    printf("Product <PNamed>:\n");
    printf("-------------------\n");
    print_pstring(&pnamed->name);
    print_pstring(&pnamed->title);
}

void from_buf_named(char **buffer, struct PNamed *pnamed) {
    from_buf_pstring(buffer, &pnamed->name);
    from_buf_pstring(buffer, &pnamed->title);
}

void to_buf_named(char **buffer, struct PNamed *pnamed) {
    to_buf_pstring(buffer, &pnamed->name);
    to_buf_pstring(buffer, &pnamed->title);
}

void ctor_named(struct PNamed *pnamed) {
    ctor_pstring(&pnamed->name);
    ctor_pstring(&pnamed->title);
}

void ctor_frompstring_named(struct PNamed* pnamed, struct PString* pname, struct PString* ptitle) {
    ctor_nomemcopy_pstring(&pnamed->name, pname->str, pname->size);
    ctor_nomemcopy_pstring(&pnamed->title, ptitle->str, ptitle->size);
}

void dtor_named(struct PNamed *pnamed) {
    dtor_pstring(&pnamed->name);
    dtor_pstring(&pnamed->title);
}

uint32_t size_named(struct PNamed *pnamed) {
    return size_in_bytes_string(&pnamed->name) + size_in_bytes_string(&pnamed->title);
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
    print_u32((uint32_t)pheader->units);
    print_u32(pheader->compress);
    print_u64(pheader->seek_info);
    print_u32(pheader->nbytes_info);
}

void ctor_file_header(struct PFileHeader *pheader) {
    pheader->begin = 100;
    pheader->units = 4;
    pheader->compress = 1;
}

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
    print_pstring(&pkey->class_name);
    print_pstring(&pkey->obj_name);
    print_pstring(&pkey->obj_title);
}

void ctor_key(struct PKey *pkey) {
    pkey->version = 4;
    ctor_fromval_datime(&pkey->date_time, 1111);
    pkey->cycle = 1;
    pkey->seek_pdir = 0;
}

void ctor_withnames_key(struct PKey *pkey, struct PString* pclass_name, struct PString *pobj_name,
                        struct PString *pobj_title) {
    ctor_key(pkey);
    ctor_nomemcopy_pstring(&pkey->class_name, pclass_name->str, pclass_name->size);
    ctor_nomemcopy_pstring(&pkey->obj_name, pobj_name->str, pobj_name->size);
    ctor_nomemcopy_pstring(&pkey->obj_title, pobj_title->str, pobj_title->size);
}

void dtor_key(struct PKey *pkey) {
    dtor_pstring(&pkey->class_name);
    dtor_pstring(&pkey->obj_name);
    dtor_pstring(&pkey->obj_title);
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
    from_buf_pstring(buffer, &(pkey->class_name));
    from_buf_pstring(buffer, &(pkey->obj_name));
    from_buf_pstring(buffer, &(pkey->obj_title));
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

    to_buf_pstring(buffer, &pkey->class_name);
    to_buf_pstring(buffer, &pkey->obj_name);
    to_buf_pstring(buffer, &pkey->obj_title);
}

uint32_t size_key(struct PKey *pkey) {
    uint32_t nbytes = 22;
    if (pkey->version > 1000) nbytes += 8;
    nbytes += size_in_bytes_string(&pkey->class_name) 
        + size_in_bytes_string(&pkey->obj_name) 
        + size_in_bytes_string(&pkey->obj_title)
        + size_datime(&pkey->date_time);
    return nbytes;
}

//
// uuid
//
void print_uuid(struct PUUID *uuid) {
    print_u32(uuid->time_low);
    print_u16(uuid->time_mid);
    print_u16(uuid->time_hi_and_version);
    print_uchar(uuid->clock_seq_hi_and_reserved);
    print_uchar(uuid->clock_seq_low);
    for (int i=0; i<6; i++)
        print_uchar((uuid->node)[i]);
}

void ctor_uuid(struct PUUID *uuid) {}

void dtor_uuid(struct PUUID *uuid) {}

void from_buf_uuid(char **buffer, struct PUUID * uuid) {
    uuid->time_low = get_u32(buffer);
    uuid->time_mid = get_u16(buffer);
    uuid->time_hi_and_version = get_u16(buffer);
    uuid->clock_seq_hi_and_reserved = get_u8(buffer);
    uuid->clock_seq_low = get_u8(buffer);
    for (int i=0; i<6; i++)
        (uuid->node)[i] = get_u8(buffer);
}

void to_buf_uuid(char **buffer, struct PUUID *uuid) {
    put_u32(buffer, uuid->time_low);
    put_u16(buffer, uuid->time_mid);
    put_u8(buffer, uuid->time_hi_and_version);
    put_u8(buffer, uuid->clock_seq_hi_and_reserved);
    put_u8(buffer, uuid->clock_seq_low);
    for (int i=0; i<6; i++)
        put_u8(buffer, (uuid->node)[i]);
}

uint32_t size_uuid(struct PUUID* ppuid) {
    return 16u;
}

//
// directory product
//
void ctor_dir(struct PDirectory *pdir) {
    pdir->version = 5;
    ctor_fromval_datime(&pdir->date_time_c, 1234);
    ctor_fromval_datime(&pdir->date_time_m, 1234);
}

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
    print_uuid(&(pdir->uuid));
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
    from_buf_uuid(buffer, &pdir->uuid);
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
    to_buf_uuid(buffer, &pdir->uuid);
    if (pdir->version <= 1000)
        for (int i=0; i<3; i++) put_u32(buffer, 0);
}

uint32_t size_dir(struct PDirectory *pdir) {
    // version (just short) + nbytes_keys + nbytes_name + seek_dir + seek_parent + seek_keys (as int)
    int nbytes = 22; 
    if (pdir->version > 1000)
        nbytes += 12;
    nbytes += size_datime(&pdir->date_time_c) + size_datime(&pdir->date_time_m) + size_uuid(&pdir->uuid);
    return nbytes;
}

uint32_t size_keylist(struct KeyList* pkeylist) {
    uint32_t nbytes = 4;
    for (int i=0; i<pkeylist->size; i++)
        nbytes += size_key(&((pkeylist->pkeys)[i]));
    return nbytes;
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

struct TopDirectory_v2 read_top_dir_v2(struct FileContext ctx) {
    struct TopDirectory_v2 root;

    // initial buffer of 300bytes is a safe assumption
    char *buffer = malloc(300);
    size_t nbytes = fread((void*)buffer, 1, 300, ctx.pfile);
    char *start = buffer;

    // get the file header
    ctor_file_header(&root.header);
    from_buf_file_header(&buffer, &root.header);

    // top dir key
    start+=100;
    ctor_key(&root.key);
    from_buf_key(&start, &root.key);

    // get named stuff
    ctor_named(&root.named);
    from_buf_named(&start, &root.named);

    // dir
    from_buf_dir(&start, &root.dir);

    return root;
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
