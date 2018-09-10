#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "aux.h"
#include "simulations.h"

void simulate_streamer_record(struct llio_t *llio) {
    // init
    llio->streamer_record.blob = NULL;

    // simulate blob
    uint32_t n_sinfo = 0u;
    int16_t version_sinfo = 5;
    struct object_t obj;
    obj.version = 2;
    obj.id = 0x1111;
    obj.bits = 0x1111;
    struct string_t str_sinfo;
    str_sinfo.size = 0;
    int byte_count = 0;
    uint32_t nbytes_sinfo = size_object(&obj) + 
                            size_in_bytes_string(&str_sinfo) +
                            2 + 4 + 4;

    // note: we do not free this resource.
    // TODO
    char *buffer_sinfo = malloc(nbytes_sinfo);
    char *tmp_sinfo = buffer_sinfo;
    put_version(&buffer_sinfo, version_sinfo);
    to_buf_object(&buffer_sinfo, &obj);
    to_buf_pstring(&buffer_sinfo, &str_sinfo);
    put_u32(&buffer_sinfo, n_sinfo);
    put_u32(&buffer_sinfo, byte_count);
    llio->streamer_record.blob = tmp_sinfo;
    
    // generate a key for that blob
    struct string_t class_name, obj_name, title_name;
    char *streamer = "StreamerInfo";
    char *tlist = "Doubly linked list";
    ctor_nomemcopy_pstring(&class_name, "TList", 5);
    ctor_nomemcopy_pstring(&obj_name, streamer, strlen(streamer));
    ctor_nomemcopy_pstring(&title_name, tlist, strlen(tlist));
    ctor_withnames_key(&llio->streamer_record.key, &class_name, &obj_name, &title_name);
    llio->streamer_record.key.seek_pdir = llio->header.begin;
    llio->streamer_record.key.key_bytes = size_key(&llio->streamer_record.key);
    llio->streamer_record.key.obj_bytes = nbytes_sinfo;
    llio->streamer_record.key.total_bytes = llio->streamer_record.key.obj_bytes +
        llio->streamer_record.key.key_bytes;

    // key and blob are ready
}

void simulate_free_segments_record(struct llio_t *llio) {
    // simulate N free segments 
    int nfree = 1;
    llio->free_segments_record.length = nfree;
    llio->free_segments_record.pfree = malloc(sizeof(struct free_t) * 
        nfree);
    ctor_pfree(llio->free_segments_record.pfree, 1000, 2000000000);

    // generate the key for that list
    struct string_t class_name;
    ctor_nomemcopy_pstring(&class_name, "TFile", 5);
    ctor_withnames_key(&llio->free_segments_record.key, &class_name, 
                       &llio->top_dir_rec.named.name, 
                       &llio->top_dir_rec.named.title);
    llio->free_segments_record.key.seek_pdir = llio->header.begin;
    llio->free_segments_record.key.key_bytes = size_key(&llio->free_segments_record.key);
    llio->free_segments_record.key.obj_bytes = size_pfree(llio->free_segments_record.pfree);
    llio->free_segments_record.key.total_bytes = llio->free_segments_record.key.key_bytes + 
        llio->free_segments_record.key.obj_bytes;

    // key and free list are ready
}
