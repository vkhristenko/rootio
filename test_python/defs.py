import ctypes

class CFunctionInfo(object):
    def __init__(self, name, res_type, args):
        self.name = name
        self.result_type = res_type
        self.arg_types = args

def wrap_cfunc(lib, fname, result, *arg_types):
    return ctypes.CFUNCTYPE(result, *arg_types)((fname, lib))

class Structure(ctypes.Structure):
    """
    def __init__(self, lib=None, *funcs_info):
        #Register all the provided c functions
        self.lib = lib
        if lib is not None:
            for func_info in funcs_info:
                self.__dict__[func_info.name] = wrap_cfunc(lib, func_info.result_type,
                    *func_info.arg_types)
                    """

    def __repr__(self):
        """print the fields"""
        res = []
        for field in self._fields_: 
            res.append("%s=%s" % (field[0], repr(getattr(self, field[0]))))
        return self.__class__.__name__ + "(" + ",".join(res) + ")"

    # TODO: understand this
    @classmethod
    def from_param(cls, obj):
        """Magically construct from a tuple"""
        if isinstance(obj, cls):
            return obj
        if isinstance(obj, tuple):
            return cls(obj)
            
        raise TypeError

if __name__ == "__main__":
    class Test(Structure):
        _fields_ = [("a", ctypes.c_int), ("b", ctypes.c_int)]

        def __init__(self, a, b, lib=None, *funcs_info):
            super(Test, self).__init__(lib, *funcs_info)
            self.a = a
            self.b = b

    a = 10
    b = 20
    test = Test(a,b)
    print test
