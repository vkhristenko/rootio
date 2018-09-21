extern crate libc;

use bootstrap::*;
use std::os::raw::c_char;
use std::ffi::CString;

mod capi {
    use std::os::raw::c_char;
    use bootstrap::*;
    #[link(name = "rootiobootstrap")]
    extern {
        // api open/close
        pub(super) fn open_to_read(filename: *const c_char) -> llio_t;
        pub(super) fn close_from_read(llio: *mut llio_t);
        pub(super) fn open_to_write(filename: *const c_char) -> llio_t;
        pub(super) fn close_from_write(llio: *mut llio_t);

        // api: read
        pub(super) fn read_keys_list_record_for_dir(llio: *const llio_t, 
                                                    dir: *const directory_t) 
            -> keys_list_record_t;
        pub(super) fn read_dir_record_by_key(llio: *const llio_t,
                                             key: *const key_t)
            -> directory_record_t;
        pub(super) fn read_generic_record_by_location(llio: *const llio_t,
                                                      location: u64)
            -> generic_record_t;

        // api: debug

    }
}

pub fn open_to_read(filename: &CString) -> llio_t {
    unsafe {
        capi::open_to_read(filename.as_ptr())
    }
}

pub fn close_from_read(mut llio: llio_t) {
    unsafe {
        capi::close_from_read(&mut llio as *mut llio_t);
    }
}

pub fn read_keys_list_record_for_dir(llio: &llio_t, dir: &directory_t) 
    -> keys_list_record_t
{
    unsafe {
        capi::read_keys_list_record_for_dir(llio as *const llio_t, dir as *const directory_t)
    }
}

pub fn read_dir_record_by_key(llio: &llio_t, key: &key_t) 
    -> directory_record_t 
{
    unsafe {
        capi::read_dir_record_by_key(llio as *const llio_t, key as *const key_t)
    }
}

pub fn read_generic_record_by_location(llio: &llio_t, location: u64)
    -> generic_record_t
{
    unsafe {
        capi::read_generic_record_by_location(llio as *const llio_t, location)
    }
}
