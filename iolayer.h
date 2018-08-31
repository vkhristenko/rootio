#ifndef iolayer_h
#define iolayer_h

#include "bootstrap.h"

//
// Records = key + blob. Continuous object
//
struct PBlobRecord {
    struct PKey key;
    void * blob;
}
struct PKeysListRecord {
    struct PKey key;
    struct KeyList keylist;
};

struct PDirectoryRecord {
    struct PKey key;
    struct PDirectory dir;
};

struct PTopDirectoryRecord {
    struct PKey pkey;
    struct PNamed pnamed;
    struct PDirectory pdir;
};

struct DirectoryBundle {
    struct PKeysListRecord keys_record;
    struct PDirectoryRecord pdir_record;
};

struct TopDirectoryBundle {
    struct PKeysListRecord keys_record;
    struct PTopDirectoryRecord root_dir_record;
};

struct PFreeSegmentRecord {
    struct PKey *pkey;
    struct PFreeNode *pnode; 
};

// for now we provide an empty streamer record (key + header of TList)
struct PStreamerInfoRecord {
    struct PKey *pkey;
};

//
// I/O Layer abstraction:
//   getting records to/from memory from/to disk
//
//   -> a record should be retrieved provided a position
//   -> a record should be written to a position where position
//      is not known by the caller. The record is updated with the location and returned
//      to the caller
//
struct IOLayer {
    uint64_t            location;
    struct FileContext  fctx;
    struct PFreeNode    frees;
};

struct Context {
    struct IOLayer         io;
    struct BootstrapBundle bootstrap;
};

//
// read records
//
struct PDirectoryRecord root_read_record_pdirectory(struct Context* ctx, 
                                               uint64_t location, uint32_t nbytes) {
    // allocate room for the whole record
    char *buffer = malloc(nbytes);

    root_seek(&ctx->fctx, location);

    root_read_buffer(ctx, &buffer, nbytes);
}

#endif // iolayer_h
