#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bootstrap.h"
#include "aux.h"

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    // next free location to write a record
    uint64_t next_location_to_write = 0;

    // open the file
    struct FileContext ctx = open_context(filename, "wb");
    struct PFileHeader header;

    // first, write the dummy header
    char buffer_array[300];
    char *buffer = buffer_array;
    memcpy(buffer, "root", 4);
    buffer+=4;
    header.begin = 100;
    header.units = 4;
    header.compress = 1;
    to_buf_file_header(&buffer, &header);

    buffer = buffer_array;
    root_write(ctx, buffer, header.begin);

    next_location_to_write += header.begin;

    // top directory record
    struct PKey key;
    struct PNamed named;
    struct PDirectory dir;

    key.version = 4;
    key.date_time.raw = 1234;
    key.cycle = 1;
    key.seek_key = header.begin;
    key.seek_pdir = 0;
    key.class_name.str = "TFile";
    key.class_name.size = 5;
    key.obj_name.str = filename;
    key.obj_name.size = strlen(filename);
    key.obj_title.size = 0;
    key.key_bytes = size_key(&key);

    named.name.str = key.obj_name.str;
    named.name.size = key.obj_name.size;
    named.title.str = key.obj_title.str;
    named.title.size = key.obj_title.size;

    dir.version = 5;
    dir.date_time_c.raw = 1234;
    dir.date_time_m.raw = 1235;
    dir.nbytes_name = size_key(&key) + size_named(&named); 
    dir.seek_dir = header.begin;
    dir.seek_parent = 0;
    
    // to get the size of bytes for the dir, we need to know the version first
    key.obj_bytes = size_dir(&dir);
    key.total_bytes = key.key_bytes + key.obj_bytes + size_named(&named);

    // key + pnamed is complete, put to buffer
    char *buf_toprec = malloc(key.total_bytes);
    char *buf_toprec_to_free = buf_toprec;
    to_buf_key(&buf_toprec, &key);
    to_buf_named(&buf_toprec, &named);
    to_buf_dir(&buf_toprec, &dir);
    root_write(ctx, buf_toprec_to_free, key.total_bytes);

    next_location_to_write += key.total_bytes;

    dir.seek_keys = next_location_to_write;

    // directory record:
    //   keys list record
    //   pdirectory record
    struct PKey key_for_keyslist;
    key_for_keyslist.version = 4;
    key_for_keyslist.date_time.raw = 12345;
    key_for_keyslist.cycle = 1;
    key_for_keyslist.seek_key = next_location_to_write;
    key_for_keyslist.seek_pdir = dir.seek_dir;
    key_for_keyslist.class_name.size = 0;
    key_for_keyslist.obj_name.size = 0;
    key_for_keyslist.obj_title.size = 0;
    key_for_keyslist.key_bytes = size_key(&key_for_keyslist);
    
    struct KeyList keylist;
    keylist.size = 0;
    key_for_keyslist.obj_bytes = size_keylist(&keylist);
    key_for_keyslist.total_bytes = key_for_keyslist.obj_bytes + key_for_keyslist.key_bytes;

    char *buf_keylist = malloc(key_for_keyslist.total_bytes);
    char *buf_keylist_to_free = buf_keylist;
    to_buf_key(&buf_keylist, &key_for_keyslist);
    put_u32(&buf_keylist, keylist.size);
    for (int i=0; i<keylist.size; i++)
        to_buf_key(&buf_keylist, &((keylist.pkeys)[i]));
    root_write(ctx, buf_keylist_to_free, key_for_keyslist.total_bytes);

    next_location_to_write += key_for_keyslist.total_bytes;

    uint64_t location_sinfo = next_location_to_write;

    dir.nbytes_keys = key_for_keyslist.total_bytes;

    // write the streamer info record
    //   key product
    //   tlist product
    struct PKey key_for_sinfo;
    key_for_sinfo.version = 4;
    key_for_sinfo.date_time.raw = 12345;
    key_for_sinfo.cycle = 1;
    key_for_sinfo.seek_key = next_location_to_write;
    key_for_sinfo.seek_pdir = dir.seek_dir;
    key_for_sinfo.class_name.size = 5;
    key_for_sinfo.class_name.str = "TList";
    key_for_sinfo.obj_name.size = 0;
    key_for_sinfo.obj_title.size = 0;
    key_for_sinfo.key_bytes = size_key(&key_for_sinfo);

    uint32_t n_sinfo = 0u;
    uint16_t version_sinfo = 5;
    struct PObject obj;
    obj.version = 2;
    obj.id = 0x1111;
    obj.bits = 0x1111;
    struct PString str_sinfo;
    str_sinfo.size = 0;
    uint32_t nbytes_sinfo = size_object(&obj) + size_in_bytes_string(&str_sinfo)
        + 2 + 4;
    key_for_sinfo.obj_bytes = nbytes_sinfo;
    key_for_sinfo.total_bytes = key_for_sinfo.obj_bytes + key_for_sinfo.key_bytes;
    char *buf_sinfo = malloc(key_for_sinfo.total_bytes);
    char *buf_sinfo_to_free = buf_sinfo;
    to_buf_key(&buf_sinfo, &key_for_sinfo);
    put_version(&buf_sinfo, version_sinfo);
    to_buf_object(&buf_sinfo, &obj);
    to_buf_pstring(&buf_sinfo, &str_sinfo);
    put_u32(&buf_sinfo, n_sinfo);
    root_write(ctx, buf_sinfo_to_free, key_for_sinfo.total_bytes);

    next_location_to_write += key_for_sinfo.total_bytes;
    uint64_t location_end = next_location_to_write;

    // write the end byte
    root_write(ctx, "b", 1);

    // write the header again
    char b[100];
    char *btmp = b;
    char *tmp = btmp;
    memcpy(btmp, "root", 4);
    btmp+=4;
    header.end = location_end;
    header.seek_info = location_sinfo;
    header.nbytes_info = key_for_sinfo.total_bytes;
    header.nbytes_free = 0;
    header.nbytes_name = dir.nbytes_name;
    to_buf_file_header(&btmp, &header);
    root_write_at_location(ctx, 0, tmp, 100);

    // write the top dir again
    char *some_b = malloc(key.total_bytes);
    char *some_b_to_free = some_b;
    to_buf_key(&some_b, &key);
    to_buf_named(&some_b, &named);
    to_buf_dir(&some_b, &dir);
    root_write(ctx, some_b_to_free, key.total_bytes);

    // close the context
    close_context(ctx);

    // free all the allocated memory
    free(buf_toprec_to_free);
    free(buf_keylist_to_free);
//    free(buf_sinfo_to_free);

    return 0;
}
