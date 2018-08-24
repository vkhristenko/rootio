sources = $(wildcard *.c)
objects = $(sources:.c=)
cc = clang
cflags = -fPIC -Wall -Wextra -O2 -g
libs = -l$(libname)

.PHONY: clean

all: $(objects)

.c:
	$(cc) $(flags) $(includedirs) -o $@ $@.c -L$(topdir) $(libs)

clean:
	rm $(objects)
