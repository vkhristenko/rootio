import ctypes
from defs import *

class CFile(ctypes.Structure):
    pass

class FileContext(Structure):
    _fields_ = [("pfile", ctypes.POINTER(CFile))]

class String(Structure):
    _fields_ = [("size", ctypes.c_int), ("str", ctypes.c_char_p)]

    def __init__(self, s):
        self.str = s
        self.size = len(s)

    @classmethod
    def from_param(cls, obj):
        """construct a String from an str"""
        if isinstance(obj, cls):
            return obj
        if isinstance(obj, str):
            return cls(obj)

        raise TypeError

class Free(Structure):
    _fields_ = [("version", ctypes.c_uint16), ("begin", ctypes.c_uint64),
                ("end", ctypes.c_uint64)]

    def __init__(self, version, begin, end):
        self.version = version
        self.begin = begin
        self.end = end

class Object(Structure):
    _fields_ = [("version", ctypes.c_uint16),
                ("id", ctypes.c_uint32),
                ("bits", ctypes.c_uint32)]

    def __init__(self, version , idd, bits):
        self.version = version
        self.id = idd
        self.bits = bits

class Datime(Structure):
    _fields_ = [("object", Object), ("raw", ctypes.c_uint32)]

    def __init__(self, raw):
        self.raw = raw

class Named(Structure):
    _fields_ = [("object", Object), 
                ("name", String),
                ("title", String)]

    def __init__(self, name, title):
        self.name = name
        self.title = title

class FileHeader(Structure):
    _fields_ = [
        ("object", Object),
        ("version", ctypes.c_uint32),
        ("begin", ctypes.c_uint32),
        ("end", ctypes.c_uint64),
        ("seek_free", ctypes.c_uint64),
        ("nbytes_free", ctypes.c_uint32),
        ("nfree", ctypes.c_uint32),
        ("units", ctypes.c_byte),
        ("compess", ctypes.c_uint32),
        ("seek_info", ctypes.c_uint64),
        ("nbytes_info", ctypes.c_uint32)
    ]

    def __init__(self, version, begin, end, seek_free, nbytes_free, nfree, units,
                 compress, seek_info, nbytes_info):
        self.version = version
        self.begin = begin
        self.end = end
        self.seek_free = seek_free
        self.nbytes_free = nbytes_free
        self.nfree = nfree
        self.units = units
        self.compress = compress
        self.seek_info = seek_info
        self.nbytes_info = nbytes_info

if __name__ == "__main__":
    context = """
    testing String class
    """
    print context
    s = String("hello")
    print s

    context = """
    testing Free class
    """
    print context 
    f = Free(1, 100, 2000)
    print f

    context = """
    testing Object class
    """
    print context
    o = Object(1, 0, 1)
    print o

    context = """
    testing Named class
    """
    print context
    n = Named(String("TDirectory"), String("root"))
    print n

    context = """
    testing File Header class
    """
    print context 
    header = FileHeader(1, 100, 2000, 1980, 10, 1, 4, 1, 1990, 10)
    print header
