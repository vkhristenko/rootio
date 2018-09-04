import ctypes

#
# decare the FileContext class
#
class File(ctypes.Structure):
    pass
class FileContext(ctypes.Structure):
    _fields_ = [("file", ctypes.POINTER(File))]

context = """opening the rootio bootstrap library"""
print context
libio = ctypes.CDLL("../librootiobootstrap.dylib")
open_context = libio.open_context
close_context = libio.close_context
dump_contents = libio.dump_contents
open_context.restype = FileContext

context = """dump the contents of a file"""
print context
filename = "/Users/vk/data/toros/test_keysdirs.root"
opts = "rb"
ctx = open_context(ctypes.c_char_p(filename), ctypes.c_char_p(opts))
dump_contents(ctx)
close_context(ctx)
