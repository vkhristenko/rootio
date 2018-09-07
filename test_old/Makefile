sources = $(wildcard *.c)
objects = $(sources:.c=)
cc = clang
cflags = -fPIC -Wall -Wextra -O2 -g
testlibs = -l$(libname) $(libs)

.PHONY: clean

all: $(objects)

.c:
	$(cc) $(flags) $(includedirs) -o $@ $@.c -L$(topdir) $(testlibs)

clean:
	rm $(objects)
