extern crate libc;
extern crate bytebuffer;
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
    let mut llio = open_to_write(&filename);
    println!("{:?}", llio);

    // simulate streamer and free segments record
    simulate_streamer_record(&mut llio);
    simulate_free_segments_record(&mut llio);

    // simulate a test record
    let mut test_record = simulate_test_record(&llio.top_dir_rec.dir);

    // simulate keys list record
    let mut keys_list_record = simulate_keys_list_record(&llio.top_dir_rec.dir, &mut test_record);

    // write record
    write_generic_record(&mut llio, &mut test_record);
    let mut dir = llio.top_dir_rec.dir;
    write_keys_list_record_for_dir(&mut llio,
                                   &mut keys_list_record,
                                   &mut dir);
    // TODO: we must coy and then reassign as we can not take 2 mutable references to the llio
    // object
    llio.top_dir_rec.dir = dir;

    // write required records
    close_from_write(llio);
}

trait Serializeable {
    fn serialize(&self) -> bytebuffer::ByteBuffer;
}

struct Test {
    x: i32,
    y: i32,
    z: i32
}

impl Serializeable for Test {
    fn serialize(&self) -> bytebuffer::ByteBuffer {
        let mut buffer = bytebuffer::ByteBuffer::new();
        buffer.write_i32(self.x);
        buffer.write_i32(self.y);
        buffer.write_i32(self.z);
        buffer
    }
}

fn simulate_keys_list_record(dir: &directory_t, gen_record: &mut generic_record_t)
    -> keys_list_record_t
{
    let class_name = CString::new("KeysList").unwrap();
    let obj_name = CString::new("").unwrap();
    let obj_title = CString::new("").unwrap();
    let mut key = key_t {
        object: object_t {version: 1, id: 1, bits: 1},
        total_bytes: 0,
        version: 1,
        obj_bytes: 0,
        date_time: datime_t {object: object_t {version: 1, id: 1, bits: 1}, raw: 12345},
        key_bytes: 0,
        cycle: 1,
        seek_key: 0,
        seek_pdir: 0,
        class_name: string_t {size: 8, cstr: class_name.as_ptr()},
        obj_name: string_t {size: 0, cstr: obj_name.as_ptr()},
        obj_title: string_t {size: 0, cstr: obj_title.as_ptr()}
    };
    key.seek_pdir = dir.seek_dir;
    key.key_bytes = size_key(&key) as u16;
    key.obj_bytes = size_key(&gen_record.key) + 4;
    key.total_bytes = key.obj_bytes + (key.key_bytes as u32);

    let record = keys_list_record_t {
        key: key,
        length: 1,
        pkeys: (&mut gen_record.key) as *mut key_t
    };

    std::mem::forget(class_name);
    std::mem::forget(obj_name);
    std::mem::forget(obj_title);
    return record
}

fn simulate_test_record(dir: &directory_t) 
    -> generic_record_t 
{
    // create a test object and serialize it
    let test = Test {
        x: 1,
        y: 2,
        z: 3
    };
    let mut blob = test.serialize().to_bytes();
    
    // generate a key for the test object
    let class_name = CString::new("Test").unwrap();
    let obj_name = CString::new("test obj name").unwrap();
    let obj_title = CString::new("test obj title").unwrap();
    let mut key = key_t {
        object: object_t {version: 1, id: 1, bits: 1},
        total_bytes: 0,
        version: 1,
        obj_bytes: 0,
        date_time: datime_t {object: object_t {version: 1, id: 1, bits: 1}, raw: 12345},
        key_bytes: 0,
        cycle: 1,
        seek_key: 0,
        seek_pdir: 0,
        class_name: string_t {size: 4, cstr: class_name.as_ptr()},
        obj_name: string_t {size: 13, cstr: obj_name.as_ptr()},
        obj_title: string_t {size: 14, cstr: obj_title.as_ptr()}
    };
    key.seek_pdir = dir.seek_dir;
    key.key_bytes = size_key(&key) as u16;
    key.obj_bytes = 12;
    key.total_bytes = key.obj_bytes + (key.key_bytes as u32);

    let record = generic_record_t {
        key: key,
        blob: (blob.as_mut_ptr()) as *mut i8
    };

    // need these guys to make rust not to clean up the memory
    std::mem::forget(class_name);
    std::mem::forget(obj_name);
    std::mem::forget(obj_title);
    std::mem::forget(blob);
    record
}
