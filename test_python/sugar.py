import ctypes
from defs import *

class CFile(ctypes.Structure):
    pass

class FileContext(Structure):
    _fields_ = [("pfile", ctypes.POINTER(CFile))]

    def __init__(self, pfile):
        self.pfile = pfile

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

class Key(Structure):
    _fields_ = [
        ("object", Object),
        ("total_bytes", ctypes.c_uint32),
        ("version", ctypes.c_int32),
        ("obj_bytes", ctypes.c_uint32),
        ("date_time", Datime),
        ("key_bytes", ctypes.c_uint16),
        ("cycle", ctypes.c_uint16),
        ("seek_key", ctypes.c_uint64),
        ("seek_pdir", ctypes.c_uint64),
        ("class_name", String),
        ("obj_name", String),
        ("obj_title", String)
    ]

    def __init__(self, total_bytes, version, obj_bytes, date_time, key_bytes,
                 cycle, seek_key, seek_pdir, class_name, obj_name, obj_title):
        self.total_bytes = total_bytes
        self.version = version
        self.obj_bytes = obj_bytes
        self.date_time = date_time
        self.key_bytes = key_bytes
        self.cycle = cycle
        self.seek_key = seek_key
        self.seek_pdir = seek_pdir
        self.class_name = class_name
        self.obj_name = obj_name
        self.obj_title = obj_title

class UUID(Structure):
    _fields_ = [
        ("version", ctypes.c_uint32),
        ("time_low", ctypes.c_uint32),
        ("time_mid", ctypes.c_uint16),
        ("time_hi_and_version", ctypes.c_uint16),
        ("clock_seq_hi_and_reserved", ctypes.c_byte),
        ("clock_seq_low", ctypes.c_ubyte),
        ("node", ctypes.c_ubyte * 6)
    ]

    def __init__(self, version, time_low, time_mid, time_hi_and_version,
                 clock_seq_hi_and_reserved, clock_seq_low, node):
        self.version = version
        self.time_low = time_low
        self.time_mid - time_mid
        self.time_hi_and_version = time_hi_and_version
        self.clock_seq_hi_and_reserved = clock_seq_hi_and_reserved
        self.clock_seq_low = clock_seq_low
        self.node = node

class Directory(Structure):
    _fields_ = [
        ("object", Object),
        ("version", ctypes.c_uint32),
        ("date_time_c", Datime),
        ("date_time_m", Datime),
        ("nbytes_keys", ctypes.c_uint32),
        ("nbytes_name", ctypes.c_uint32),
        ("seek_dir", ctypes.c_uint64),
        ("seek_parent", ctypes.c_uint64),
        ("seek_keys", ctypes.c_uint64),
        ("uuid", UUID)
    ]

    def __init__(self, version, date_time_c, date_time_m, nbytes_keys, nbytes_name,
                 seek_dir, seek_parent, seek_keys, uuid):
        self.version = version
        self.date_time_c = date_time_c
        self.date_time_m = date_time_m
        self.nbytes_keys = nbytes_keys
        self.nbytes_name = nbytes_name
        self.seek_dir = seek_dir
        self.seek_parent = seek_parent
        self.seek_keys = seek_keys
        self.uuid = uuid

class TopDirectoryRecord(Structure):
    _fields_ = [
        ("key", Key),
        ("named", Named),
        ("dir", Directory)
    ]

    def __init__(self, key, named, d):
        self.key = key
        self.named = named
        self.dir = d

class DirectoryRecord(Structure):
    _fields_ = [
        ("key", Key),
        ("dir", Directory)
    ]

    def __init__(self, key, d):
        self.key = key
        self.dir = d

class KeysListRecord(Structure):
    _fields_ = [
        ("key", Key),
        ("length", ctypes.c_int),
        ("pkeys", ctypes.POINTER(Key))
    ]

    def __init__(self, key, length, pkeys):
        self.key = key
        self.length = length
        self.pkeys = pkeys

class GenericRecord(Structure):
    _fields_ = [
        ("key", Key),
        ("blob", ctypes.c_char_p)
    ]

    def __init__(self, key, blob):
        self.key = key
        self.blob = blob

class StreamerRecord(Structure):
    _fields_ = [
        ("key", Key),
        ("blob", ctypes.c_char_p)
    ]

    def __init__(self, key, blob):
        self.key = key
        self.blob = blob

class FreeSegmentsRecord(Structure):
    _fields_ = [
        ("key", Key),
        ("length", ctypes.c_int),
        ("pfree", ctypes.POINTER(Free))
    ]

    def __init__(self, key, length, pfree):
        self.key = key
        self.length = length
        self.pfree = pfree

class LLIO(Structure):
    _fields_ = [
        ("location", ctypes.c_uint64),
        ("fctx", FileContext),
        ("header", FileHeader),
        ("top_dir_rec", TopDirectoryRecord),
        ("streamer_record", StreamerRecord),
        ("free_segments_record", FreeSegmentsRecord)
    ]

    def __init__(self, location, fctx, header, top_dir_rec, streamer_record,
                 free_segments_record):
        self.location = location
        self.fctx = fctx
        self.header = header
        self.top_dir_rec = top_dir_rec
        self.streamer_record = streamer_record
        self.free_segments_record = free_segments_record

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

    context = """
    testing Key clas
    """
    print context 
    key = Key(100, 1, 10, Datime(12345), 122, 1, 10000, 10000, String("abcde"), 
              String("aaaa"), String("bbbb"))
    print key

    context = """
    testing uuid_t class
    """
    print context 
    cbytes = ctypes.c_ubyte*6
    uuid = UUID(1, 1234, 123, 123, 15, 15, cbytes(*[1,2,3,5,6,6]))
    print uuid

    context = """
    testing Directory class
    """
    print context 
    d = Directory(1, Datime(1234), Datime(1234), 1000, 2000, 100, 150, 200, uuid)
    print d

    context = """
    testing Top Directory Record
    """
    print context
    top_dir_record = TopDirectoryRecord(key, n, d)
    print top_dir_record

    context = """
    testing Direcoty Record
    """
    print context
    dir_record = DirectoryRecord(key, d)
    print dir_record

    context = """
    testing Keys List Record
    """
    print context
    n = 5
    keys_list_type = Key*n
    keys_list = keys_list_type(*[key, key, key, key, key])
    keys_list_record = KeysListRecord(key, n, keys_list)
    print keys_list_record
    for i in range(n):
        print keys_list_record.pkeys[i]


    context = """
    testing generic record class
    """
    print context
    n = 100
    gen_record = GenericRecord(key, "blobblobblob")
    print gen_record
    
    context = """
    testing streamer record class
    """
    print context
    n = 100
    streamer_record = StreamerRecord(key, "streamerstreamerstreamer")
    print streamer_record

    context = """
    testing free segments record
    """
    print context 
    free_types = Free * 10
    frees = free_types(*[Free(1, 100, 200) for i in range(10)])
    free_segments_record = FreeSegmentsRecord(key, 10, frees)
    print free_segments_record

    context = """
    testing llio_t class
    """
    print context
    llio = LLIO(0, FileContext(ctypes.pointer(CFile())), header, top_dir_record,
                streamer_record, free_segments_record)
    print llio
