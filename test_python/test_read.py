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
    def recurse(llio, d):
        keys_list_record = read_keys_list_record_for_dir(ctypes.pointer(llio),
                                                         ctypes.pointer(d))
        print keys_list_record
        for i in range(keys_list_record.length):
            key = keys_list_record.pkeys[i]
            print key

            if key.class_name.str == "TDirectory":
                dir_record = read_dir_record_by_key(ctypes.pointer(llio),
                                                    ctypes.pointer(key))
                print dir_record
                recurse(llio, dir_record.dir)
    # launch
    recurse(llio, llio.top_dir_rec.dir)
    close_from_read(ctypes.pointer(llio))
