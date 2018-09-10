import ctypes
import sys

from sugar import *
from defs import *

if __name__ == "__main__":
    context = """
    test_write functionality
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
    print llio

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

    class Test(object):
        def __init__(self, a, b, c):
            self.a = a
            self.b = b
            self.c = c


    # write all the needed records
    close_from_write(ctypes.pointer(llio))
