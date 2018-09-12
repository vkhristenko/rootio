extern crate libc;

use bootstrap::llio_t;
use std::os::raw::c_char;
use std::ffi::CString;

mod capi {
    use std::os::raw::c_char;
    use bootstrap::llio_t;
    #[link(name = "rootiobootstrap")]
    extern {
        pub(super) fn open_to_read(filename: *const c_char) -> llio_t;
        pub(super) fn close_from_read(llio: *mut llio_t);
    }
}

pub fn open_to_read(filename: &CString) -> llio_t {
    unsafe {
        capi::open_to_read(filename.as_ptr())
    }
}

pub fn close_from_read(llio: &mut llio_t) {
    unsafe {
        capi::close_from_read(&mut llio as *mut llio_t);
    }
}
