sources = $(wildcard *.c)
objects = $(sources:.c=.o)
includedirs = -I$(shell pwd)
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

.PHONY: tests clean

all: $(lib) tests

$(lib): $(objects)
	$(cc) $(ldflags) -o $@ $^

$.o: %.c
	$(cc) $(cflags) -c -o $@ $^

tests:
	cd tests && $(MAKE)

clean:
	rm $(objects) $(lib)
	cd tests && $(MAKE) clean
