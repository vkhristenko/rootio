extern crate libc;
extern crate test_rust;

use std::ffi::CString;
use std::os::raw::c_char;

use test_rust::*;

fn main() {
    println!("hello world!");

    // create an object_tobject_t and print
    let obj = object_t {version: 1, id: 1, bits: 1};

    // create a string_t objectd and print
    let mut s = CString::new("abc").unwrap();
    let string = string_t {size: 10, cstr: s.as_ptr()};

    // create a free_t and print
    let ifree = free_t {version: 1, begin: 100, end: 2000};

    // create a dateme_t and print
    let dt = datime_t {object: obj, raw: 1};

    // create a named_t and print
    let n = named_t {
        object: obj,
        name: string,
        title: string,
    };
}
