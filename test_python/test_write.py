import ctypes
import sys
import struct

from sugar import *
from defs import *

if __name__ == "__main__":
    context = """
    begin testing 'test_write' functionality
    """
    print context
    filename = sys.argv[1]
    path2lib = sys.argv[2]

    print ("filename = %s" % filename)
    print ("path2lib = %s" % path2lib)

    lib = ctypes.CDLL(path2lib)
    open_to_write = wrap_cfunc(lib, "open_to_write", LLIO, ctypes.c_char_p)
    close_from_write = wrap_cfunc(lib, "close_from_write", ctypes.POINTER(LLIO))
    llio = open_to_write(filename)
#    print llio

    context = """
    get all the cfunctions
    """
    print context
    write_generic_record = wrap_cfunc(lib, "write_generic_record",
        None, ctypes.POINTER(LLIO), ctypes.POINTER(GenericRecord))
    write_keys_list_record_for_dir = wrap_cfunc(lib, "write_keys_list_record_for_dir",
        None, ctypes.POINTER(LLIO), ctypes.POINTER(KeysListRecord),
        ctypes.POINTER(Directory))
    simulate_streamer_record = wrap_cfunc(lib, "simulate_streamer_record",
        None, ctypes.POINTER(LLIO))
    simulate_free_segments_record = wrap_cfunc(lib, "simulate_free_segments_record",
        None, ctypes.POINTER(LLIO))
    size_object = wrap_cfunc(lib, "size_object", 
        ctypes.c_uint32, ctypes.POINTER(Object))
    size_in_bytes_string = wrap_cfunc(lib, "size_in_bytes_string",
        ctypes.c_uint32, ctypes.POINTER(String))
    size_key = wrap_cfunc(lib, "size_key",
        ctypes.c_uint32, ctypes.POINTER(Key))

    class Test(object):
        def __init__(self, a, b, c):
            self.a = a
            self.b = b
            self.c = c

        def ser(self):
            return struct.pack(">I", self.a) +\
                   struct.pack(">I", self.b) +\
                   struct.pack(">I", self.c)

        def size(self):
            return 12

    def simulate_test_record(llio, d):
        # create a blob
        test = Test(1,2,3)
        buf = test.ser()

        # create a key
        class_name = "Test"
        obj_name = "test obj name"
        obj_title = "test obj title"

        key = Key(total_bytes=0, 
                  version=1,
                  obj_bytes=0,
                  date_time=Datime(12345),
                  key_bytes = 0,
                  cycle = 1,
                  seek_key = 0,
                  seek_pdir = 0,
                  class_name=String(class_name), 
                  obj_name=String(obj_name), 
                  obj_title=String(obj_title))
        key.seek_pdir = d.seek_dir
        key.key_bytes = size_key(ctypes.pointer(key))
        key.obj_bytes = test.size()
        key.total_bytes = key.obj_bytes + key.key_bytes

        return GenericRecord(key, buf)

    def simulate_keys_list_record_for_dir(llio, d, gen_record):
        # create a key for the record
        key = Key(total_bytes=0, 
                  version=1,
                  obj_bytes=0,
                  date_time=Datime(12345),
                  key_bytes = 0,
                  cycle = 1,
                  seek_key = 0,
                  seek_pdir = 0,
                  class_name=String("KeysList"), 
                  obj_name=String(""), 
                  obj_title=String(""))
        key.seek_pdir = d.seek_dir
        key.key_bytes = size_key(ctypes.pointer(key))
        key.obj_bytes = size_key(ctypes.pointer(gen_record.key)) + 4
        key.total_bytes = key.obj_bytes + key.key_bytes

        return KeysListRecord(key, 1, ctypes.pointer(gen_record.key))

    # simulate the records
    simulate_streamer_record(ctypes.pointer(llio))
    simulate_free_segments_record(ctypes.pointer(llio))
    test_record = simulate_test_record(llio, llio.top_dir_rec.dir)
    keys_list_record = simulate_keys_list_record_for_dir(
        llio, llio.top_dir_rec.dir, test_record)

    # write the records
    write_generic_record(ctypes.pointer(llio), ctypes.pointer(test_record))
    write_keys_list_record_for_dir(ctypes.pointer(llio), 
                                   ctypes.pointer(keys_list_record),
                                   ctypes.pointer(llio.top_dir_rec.dir))

    # write all the needed records
    close_from_write(ctypes.pointer(llio))

