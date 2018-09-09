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
        GenericRecord, ctypes.c_uint64)

    location = llio.header.begin
    irec = 0
    while location < llio.header.end:
        gen_record = read_generic_record_by_location(ctypes.pointer(llio), 
            location)
        print gen_record.key

    # close the file
    close_from_read(ctypes.pointer(llio))
