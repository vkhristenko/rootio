extern crate libc;

use std::ffi::CString;
use std::os::raw::c_char;

#[repr(C)]
struct FileContext {
    file: *mut libc::c_void
}

impl Copy for FileContext {}
impl Clone for FileContext {
    fn clone(&self) -> FileContext {
        *self
    }
}

#[link(name = "rootiobootstrap")]
extern {
    fn open_context(filename: *const c_char, opts: *const c_char) -> FileContext;
    fn dump_contents(ctx: FileContext);
    fn close_context(ctx: FileContext);
}

fn main() {
    println!("hello world!");

    unsafe {
        let filename = CString::new("/Users/vk/data/toros/test_keysdirs.root").unwrap();
        let opts = CString::new("rb").unwrap();
        let ctx = open_context(filename.as_ptr(), opts.as_ptr());
        dump_contents(ctx);
        close_context(ctx);
    }
}
