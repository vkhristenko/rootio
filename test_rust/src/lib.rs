extern crate libc;

use std::ffi::CString;
use std::os::raw::c_char;

#[repr(C)]
#[derive(Debug)]
pub struct file_context_t {
    pub file: *mut libc::c_void
}

impl Copy for file_context_t {}
impl Clone for file_context_t {
    fn clone(&self) -> file_context_t {
        *self
    }
}

#[repr(C)]
#[derive(Debug)]
pub struct string_t {
    pub size: i32,
    pub cstr: * const c_char 
}

impl Copy for string_t {}
impl Clone for string_t {
    fn clone(&self) -> string_t {
        *self
    }
}

#[repr(C)]
#[derive(Debug)]
pub struct free_t {
    pub version: u16,
    pub begin: u16,
    pub end: u16
}

impl Copy for free_t {}
impl Clone for free_t {
    fn clone(&self) -> free_t {
        *self
    }
}

#[repr(C)]
#[derive(Debug)]
pub struct object_t {
    pub version: u16,
    pub id: u32,
    pub bits: u32
}

impl Copy for object_t {}
impl Clone for object_t {
    fn clone(&self) -> object_t {
        *self
    }
}

#[repr(C)]
#[derive(Debug)]
pub struct datime_t {
    pub object: object_t,
    pub raw: u32
}

impl Copy for datime_t {}
impl Clone for datime_t {
    fn clone(&self) -> datime_t {
        *self
    }
}

#[repr(C)]
#[derive(Debug)]
pub struct named_t {
    pub object: object_t,
    pub name: string_t,
    pub title: string_t
}

#[repr(C)]
#[derive(Debug)]
pub struct file_header_t {
    pub object: object_t,
    pub version: u32,
    pub begin: u32,
    pub end: u64,
    pub seek_free: u64,
    pub nbytes_free: u32,
    pub nfree: u32,
    pub nbytes_name: u32,
    pub units: c_char,
    pub compress: u32,
    pub seek_info: u64,
    pub nbytes_info: u32
}

#[repr(C)]
#[derive(Debug)]
pub struct key_t {
    pub object: object_t,
    pub total_bytes: u32,
    pub version: i32,
    pub obj_bytes: u32,
    pub date_time: datime_t,
    pub key_bytes: u16,
    pub cycle: u16, 
    pub seek_key: u64,
    pub seek_pdir: u64,
    pub class_name: string_t,
    pub obj_name: string_t,
    pub obj_title: string_t
}

#[repr(C)]
#[derive(Debug)]
pub struct uuid_t {
    pub version: u32,
    pub time_low: u32,
    pub time_mid: u16,
    pub time_hi_and_version: u16,
    pub clock_seq_hi_and_reserved: u8,
    pub clock_seq_low: u8,
    pub node: [u8; 6]
}

#[repr(C)]
#[derive(Debug)]
pub struct directory_t {
    pub object: object_t,
    pub version: u32,
    pub date_time_c: datime_t,
    pub date_time_m: datime_t,
    pub nbytes_keys: u32,
    pub nbytes_name: u32,
    pub seek_dir: u64,
    pub seek_parent: u64,
    pub seek_keys: u64,
    pub uuid: uuid_t
}
