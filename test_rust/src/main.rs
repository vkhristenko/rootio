extern crate libc;

use std::ffi::CString;
use std::os::raw::c_char;

#[repr(C)]
struct file_context_t {
    file: *mut libc::c_void
}

impl Copy for file_context_t {}
impl Clone for file_context_t {
    fn clone(&self) -> file_context_t {
        *self
    }
}

#[repr(C)]
struct string_t {
    size: i32,
    cstr: * const c_char 
}

#[repr(C)]
struct free_t {
    version: u16,
    begin: u16,
    end: u16
}

#[repr(C)]
struct object_t {
    version: u16,
    id: u32,
    bits: u32
}

#[repr(C)]
struct datime_t {
    object: object_t,
    raw: u32
}

#[repr(C)]
struct named_t {
    object: object_t,
    name: string_t,
    title: string_t
}

#[repr(C)]
struct file_header_t {
    object: object_t,
    version: u32,
    begin: u32,
    end: u64,
    seek_free: u64,
    nbytes_free: u32,
    nfree: u32,
    nbytes_name: u32,
    units: c_char,
    compress: u32,
    seek_info: u64,
    nbytes_info: u32
}

#[repr(C)]
struct key_t {
    object: object_t,
    total_bytes: u32,
    version: i32,
    obj_bytes: u32,
    date_time: datime_t,
    key_bytes: u16,
    cycle: u16, 
    seek_key: u64,
    seek_pdir: u64,
    class_name: string_t,
    obj_name: string_t,
    obj_title: string_t
}

#[repr(C)]
struct uuid_t {
    version: u32,
    time_low: u32,
    time_mid: u16,
    time_hi_and_version: u16,
    clock_seq_hi_and_reserved: u8,
    clock_seq_low: u8,
    node: [u8; 6]
}

#[repr(C)]
struct directory_t {
    object: object_t,
    version: u32,
    date_time_c: datime_t,
    date_time_m: datime_t,
    nbytes_keys: u32,
    nbytes_name: u32,
    seek_dir: u64,
    seek_parent: u64,
    seek_keys: u64,
    uuid: uuid_t
}

/*
#[link(name = "rootiobootstrap")]
extern {
    fn open_context(filename: *const c_char, opts: *const c_char) -> FileContext;
    fn dump_contents(ctx: FileContext);
    fn close_context(ctx: FileContext);
}*/

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

/*
    unsafe {
        let filename = CString::new("/Users/vk/data/toros/test_keysdirs.root").unwrap();
        let opts = CString::new("rb").unwrap();
        let ctx = open_context(filename.as_ptr(), opts.as_ptr());
        dump_contents(ctx);
        close_context(ctx);
    }
    */
}
