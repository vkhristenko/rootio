extern crate libc;
extern crate test_rust;

use std::ffi::CString;
use std::ffi::CStr;
use std::os::raw::c_char;
use std::slice;
use std::env;

use test_rust::bootstrap::*;
use test_rust::iolayer::*;

fn main() {
    println!("hello world!");

    let args: Vec<String> = env::args().collect();

    // create an object_tobject_t and print
    let obj = object_t {version: 1, id: 1, bits: 1};
    println!("{:?}", obj);

    // create a string_t objectd and print
    let s = CString::new("abc").unwrap();
    let string = string_t {size: 10, cstr: s.as_ptr()};
    println!("{:?}", s);

    // create a free_t and print
    let ifree = free_t {version: 1, begin: 100, end: 2000};
    println!("{:?}", ifree);

    // create a dateme_t and print
    let dt = datime_t {object: obj, raw: 1};
    println!("{:?}", dt);

    // create a named_t and print
    let n = named_t {
        object: obj,
        name: string,
        title: string,
    };
    println!("{:?}", n);

    // create a file_header_t and print
    let file_header = file_header_t {
        object: obj,
        version: 1,
        begin: 100,
        end: 2000,
        seek_free: 500,
        nbytes_free: 100,
        nfree: 10,
        nbytes_name: 100,
        units: 4,
        compress: 4,
        seek_info: 100,
        nbytes_info: 300
    };
    println!("{:?}", file_header);

    // create a key_t and print
    let key = key_t {
        object: obj,
        total_bytes: 100,
        version: 1,
        obj_bytes: 100,
        date_time: datime_t { object: obj, raw: 12345},
        key_bytes: 10,
        cycle: 1,
        seek_key: 100,
        seek_pdir: 10,
        class_name: string_t { size: 10, cstr: s.as_ptr()},
        obj_name: string_t { size: 10, cstr: s.as_ptr() },
        obj_title: string_t { size: 10, cstr: s.as_ptr()}
    };
    println!("{:?}", key);

    // create a uuid_t and print
    let uuid = uuid_t {
        version: 0,
        time_low: 1,
        time_mid: 2,
        time_hi_and_version: 3,
        clock_seq_hi_and_reserved: 4,
        clock_seq_low: 5,
        node: [0,1,2,3,4,5]
    };
    println!("{:?}", uuid);

    // crate a directory_t and print
    let dir = directory_t {
        object: obj,
        version: 1,
        date_time_c: datime_t {object: obj, raw: 1111},
        date_time_m: datime_t {object: obj, raw: 2222},
        nbytes_keys: 100,
        nbytes_name: 200,
        seek_dir: 300,
        seek_parent: 400,
        seek_keys: 500,
        uuid: uuid
    };
    println!("{:?}", dir);

    // create a top dir record and print
    let top_dir_rec = top_dir_record_t {
        key: key,
        named: n,
        dir: dir
    };
    println!("{:?}", top_dir_rec);

    // create an llio_t and close
    let fname = args[1].clone();
    let filename = CString::new().unwrap();
    let mut llio = open_to_read(&filename);
    println!("{:?}", llio);
    recurse(&llio, &llio.top_dir_rec.dir);
    close_from_read(llio);
}

fn recurse(llio: &llio_t, dir: &directory_t) {
    let keys_list_record = read_keys_list_record_for_dir(&llio, &dir);
    println!("{:?}", keys_list_record);

    let keys = unsafe {
        slice::from_raw_parts(keys_list_record.pkeys, 
                              keys_list_record.length as usize)
    };

    for key in keys {
        println!("{:?}", key);
        let class_name = unsafe {
            CStr::from_ptr(key.class_name.cstr)
        };
        if class_name == CStr::from_bytes_with_nul(b"TDirectory\0").unwrap() {
            println!("class_name = {:?}", class_name);
            let dir_record = read_dir_record_by_key(&llio, &key);
            println!("{:?}", dir_record);
            recurse(&llio, &dir_record.dir);
        }
    }
}
