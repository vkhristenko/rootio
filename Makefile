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

.PHONY: tests clean

all: $(lib) tests

$(lib): $(objects)
	$(cc) $(ldflags) -o $@ $^ $(libs)

$.o: %.c
	$(cc) $(cflags) -c -o $@ $^

tests:
	cd tests && $(MAKE)

clean:
	rm $(objects) $(lib)
	cd tests && $(MAKE) clean
