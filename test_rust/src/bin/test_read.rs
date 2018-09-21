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

    // create an llio_t and close
    let fname = args[1].clone();
    let filename = CString::new(fname).unwrap();
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
