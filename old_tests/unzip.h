#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "zlib.h"

 #define CHECK_ERR(err, msg) { \
    if (err != Z_OK) { \
        fprintf(stderr, "%s error: %d\n", msg, err); \
        exit(1); \
    } \
}

void unzip(char **dest, char const* src, int compressed_size, int decompressed_size) {
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;

    infstream.avail_in = compressed_size - 9;
    infstream.next_in = (Bytef*) (src + 9);
    infstream.next_out = (Bytef*) *dest;
    infstream.avail_out = decompressed_size;
    
    // initialize
    int err = inflateInit(&infstream);
    CHECK_ERR(err, "inflateInit");

    // decompress
    do {
        err = inflate(&infstream, Z_NO_FLUSH);
    } while (infstream.avail_out != 0);

    err = inflateEnd(&infstream);
    CHECK_ERR(err, "inflateEnd");
}
