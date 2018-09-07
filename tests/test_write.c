#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iolayer.h"
#include "aux.h"

struct generic_record_t simulate_keylist_record(struct directory_t* pdir) {
    struct generic_record_t record;

    return record;
}

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

struct test_t {
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

struct test_record_t {
    struct key_t key;
    struct test_t test;
};

void serialize_test(char **dest, struct test_t * test) {
    put_u32(dest, test->x);
    put_u32(dest, test->y);
    put_u32(dest, test->z);
}

uint32_t size_test(struct test_t *test) {
    return 12;
}

struct generic_record_t simulate_test_record(struct llio_t *llio, struct directory_t *dir) {
    // initialize the test object
    struct generic_record_t record;
    struct test_t test;
    test.x = 1;
    test.y = 2;
    test.z = 3;
    char b[12]; char *buffer = b;
    serialize_test(&buffer, &test);
    record.blob = b;

    // generate the key
    struct string_t class_name, obj_name, obj_title;
    ctor_nomemcopy_pstring(&class_name, "test_t", 6);
    ctor_nomemcopy_pstring(&obj_name, "test obj name", 13);
    ctor_nomemcopy_pstring(&obj_title, "test obj title", 14);
    ctor_withnames_key(&record.key, 
                       &class_name,
                       &obj_name,
                       &obj_title);
    record.key.seek_pdir = dir->seek_dir;
    record.key.key_bytes = size_key(&record.key);
    record.key.obj_bytes = size_test(&test);
    record.key.total_bytes = record.key.obj_bytes = record.key.key_bytes;

    return record;
}

struct keys_list_record_t simulate_keys_list_record_for_dir(struct llio_t *llio, 
                                                            struct directory_t *dir,
                                                            struct generic_record_t *rec) {
    // init
    struct keys_list_record_t record; 
    record.length = 1;
    record.pkeys = &rec->key;

    // generate key
    /*
    struct PString class_name;
    ctor_withnames_key(&record.key, 
                       &llio->top_dir_rec.key.class_name, 
                       &llio->top_dir_rec.named.name, 
                       &llio->top_dir_rec.named.title);
                       */
    ctor_key(&record.key);
    record.key.seek_pdir = dir->seek_dir;
    record.key.key_bytes = size_key(&record.key);
    record.key.obj_bytes = size_key(&rec->key) + 4;
    record.key.total_bytes = record.key.obj_bytes + record.key.key_bytes;

    return record;
}

int main(int argc, char** argv) {
    if (argc==1) {
        printf("no input file provided\n");
        return 0;
    }

    char *filename = argv[1];
    printf("filename = %s\n", filename);

    struct llio_t llio = open_to_write(filename);

    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes);

    // we simulate the streamer and free segments records
    simulate_streamer_record(&llio);
    simulate_free_segments_record(&llio);

    printf("--- simulated streamer, free segments and top dir keys list records ---\n");

    print_key(&llio.streamer_record.key);
    printf("- - - - - - - - - - - - - -\n");
    print_key(&llio.free_segments_record.key);
    printf("- - - - - - - - - - - - - -\n");

    struct generic_record_t test_record = simulate_test_record(&llio, 
        &llio.top_dir_rec.dir);
    struct keys_list_record_t keys_list_record = simulate_keys_list_record_for_dir(
        &llio, &llio.top_dir_rec.dir, &test_record);
    printf("- - - - - - - - - - - - - -\n");
    printf("print test record key\n");
    printf("- - - - - - - - - - - - - -\n");
    print_key(&test_record.key);
    printf("- - - - - - - - - - - - - -\n");
    printf("print keys list key\n");
    printf("- - - - - - - - - - - - - -\n");
    print_key(&keys_list_record.key);
    printf("- - - - - - - - - - - - - -\n");
    write_generic_record(&llio, &test_record);

    // write the keys list for the root/top dir
    write_keys_list_record_for_dir(&llio, &keys_list_record, &llio.top_dir_rec.dir);
    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes + 
           keys_list_record.key.total_bytes + test_record.key.total_bytes);
    
    /**
     * this is left for testing purposes
     *
    // sequence of actions to do upon closing the file
    write_streamer_record(&llio);
    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes +
           keys_list_record.key.total_bytes +
           llio.streamer_record.key.total_bytes);

    write_free_segments_record(&llio);
    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes + 
           keys_list_record.key.total_bytes +
           llio.streamer_record.key.total_bytes + 
           llio.free_segments_record.key.total_bytes);

    write_end_byte(&llio);
    printf("location to write = %lu and should be %d\n",
           llio.location, 100 + llio.top_dir_rec.key.total_bytes +
           keys_list_record.key.total_bytes +
           llio.streamer_record.key.total_bytes + 
           llio.free_segments_record.key.total_bytes + 1);

    write_top_dir_record(&llio);
    write_file_header(&llio);
    */
    close_from_write(&llio);
    
    print_key(&keys_list_record.key);

    return 0;
}
