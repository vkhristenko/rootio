#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "iolayer.h"
#include "simulations.h"
#include "aux.h"

struct generic_record_t simulate_keylist_record(struct directory_t* pdir) {
    struct generic_record_t record;

    return record;
}

struct test_t {
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

struct test_record_t {
    struct rkey_t key;
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

struct generic_record_t simulate_test_record(struct llio_t *llio, struct rdirectory_t *dir) {
    // initialize the test object
    struct generic_record_t record;
    struct test_t test;
    test.x = 1;
    test.y = 2;
    test.z = 3;
    char b[12]; char *buffer = b;
    serialize_test(&buffer, &test);
    record.blob = malloc(12);
    memcpy(record.blob, buffer, 12);

    // generate the key
    struct rstring_t class_name, obj_name, obj_title;
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
                                                            struct rdirectory_t *dir,
                                                            struct generic_record_t *rec) {
    // init
    struct keys_list_record_t record; 
    record.length = 1;
    record.pkeys = &rec->key;

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
    
    close_from_write(&llio);
    
    print_key(&keys_list_record.key);

    return 0;
}
