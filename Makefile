sources = $(wildcard *.c)
objects = $(sources:.c=.o)
includedirs = -I$(shell pwd)
libs = -lz
os = $(shell uname)
topdir = $(shell pwd)
extension = so
ifeq ($(os), Darwin)
	extension = dylib
endif
cc = clang
cflags = -fPIC -Wall -Wextra -O2 -g # C flags
ldflags = -shared
libname = rootiobootstrap
lib = lib$(libname).$(extension)

export lib
export libname
export topdir
export includedirs
export libs

.PHONY: test_c clean

all: $(lib) test_c

$(lib): $(objects)
	$(cc) $(ldflags) -o $@ $^ $(libs)

$.o: %.c
	$(cc) $(cflags) -c -o $@ $^

test_c:
	cd test_c && $(MAKE)

clean:
	rm $(objects) $(lib)
	cd test_c && $(MAKE) clean
