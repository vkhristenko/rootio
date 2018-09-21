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
    let llio = open_to_read(&filename);
    println!("{:?}", llio);

    let mut location: u64 = llio.header.begin as u64;
    let mut irecord = 0;
    while location < llio.header.end {
        let generic_record = read_generic_record_by_location(&llio, location);
        print_key_info(&generic_record.key);

        location += generic_record.key.total_bytes as u64;
        irecord+=1;
    }

    close_from_read(llio);
}

fn print_key_info(key: &key_t) {
    println!("{:?}", key);
}
