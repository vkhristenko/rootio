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

        // api: write
        pub(super) fn write_generic_record(llio: *mut llio_t, record: *mut generic_record_t);
        pub(super) fn write_keys_list_record_for_dir(llio: *mut llio_t, 
                                                     keys_list_record: *mut keys_list_record_t,
                                                     dir: *mut directory_t);

        // api: simulations
        pub(super) fn simulate_streamer_record(llio: *mut llio_t);
        pub(super) fn simulate_free_segments_record(llio: *mut llio_t);

        // api: getters
        pub(super) fn size_key(key: *const key_t) -> u32;

        // api: debug

    }
}

pub fn write_generic_record(llio: &mut llio_t, record: &mut generic_record_t) {
    unsafe {
        capi::write_generic_record(llio as *mut llio_t,
                                   record as *mut generic_record_t);
    }
}

pub fn write_keys_list_record_for_dir(llio: &mut llio_t, keys_list_record: &mut keys_list_record_t,
                                      dir: &mut directory_t) {
    unsafe {
        capi::write_keys_list_record_for_dir(llio as *mut llio_t,
                                                 keys_list_record as *mut keys_list_record_t,
                                                 dir as *mut directory_t);
    }
}

pub fn size_key(key: &key_t) -> u32 {
    unsafe {
        capi::size_key(key as *const key_t)
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

pub fn open_to_write(filename: &CString) -> llio_t {
    unsafe {
        capi::open_to_write(filename.as_ptr())
    }
}

pub fn close_from_write(mut llio: llio_t) {
    unsafe {
        capi::close_from_write(&mut llio as *mut llio_t);
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

pub fn simulate_streamer_record(llio: &mut llio_t) {
    unsafe {
        capi::simulate_streamer_record(llio as *mut llio_t);
    }
}

pub fn simulate_free_segments_record(llio: &mut llio_t) {
    unsafe {
        capi::simulate_free_segments_record(llio as *mut llio_t);
    }
}
