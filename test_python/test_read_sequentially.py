import ctypes
import sys

from sugar import *
from defs import *

if __name__ == "__main__":
    context = """
    test_read functionality: recursively dump the contents of the file
    """
    print context
    filename = sys.argv[1]
    path2lib = sys.argv[2]

    print ("filename = %s" % filename)
    print ("path2lib = %s" % path2lib)

    lib = ctypes.CDLL(path2lib)
    open_to_read = wrap_cfunc(lib, "open_to_read", LLIO, ctypes.c_char_p)
    close_from_read = wrap_cfunc(lib, "close_from_read", ctypes.POINTER(LLIO))
    llio = open_to_read(filename)
    print llio

    read_keys_list_record_for_dir = wrap_cfunc(lib, "read_keys_list_record_for_dir",
        KeysListRecord, ctypes.POINTER(LLIO),
        ctypes.POINTER(Directory))
    read_dir_record_by_key = wrap_cfunc(lib, "read_dir_record_by_key",
        DirectoryRecord, ctypes.POINTER(LLIO),
        ctypes.POINTER(Key))
    read_generic_record_by_location = wrap_cfunc(lib, "read_generic_record_by_location",
        GenericRecord, ctypes.POINTER(LLIO), ctypes.c_uint64)
    print read_generic_record_by_location.argtypes

    def print_key_info(key):
        s = "At:{:>10} N:{:>10} {:>15} CX={:2.2f}".format(key.seek_key,
                key.total_bytes, 
                "" if key.class_name.size==0 else key.class_name.str[:key.class_name.size], 
                key.obj_bytes / (key.total_bytes - key.key_bytes))
        print s

    location = long(llio.header.begin)
    irec = 0
    print ("location = {}".format(location))
    print ("end = {}".format(llio.header.end))
    while location < llio.header.end:
        gen_record = read_generic_record_by_location(ctypes.pointer(llio), 
            ctypes.c_uint64(location))
        print_key_info(gen_record.key)

        #
        location += gen_record.key.total_bytes
        irec += 1

    # close the file
    close_from_read(ctypes.pointer(llio))
