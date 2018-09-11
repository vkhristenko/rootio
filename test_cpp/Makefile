sources = $(wildcard *.cpp)
objects = $(sources:.cpp=)
cxx = clang++
cxxflags = -fPIC -Wall -Wextra -O2 --std=c++17
rootcflags = $(shell root-config --cflags)
rootldflags = $(shell root-config --ldflags)
testlibs = -l$(libname) $(libs) $(shell root-config --libs)

.PHONY: clean

all: $(objects)

.cpp:
	$(cxx) $(rootcflags) $(cxxflags) $(includedirs) -o $@ $@.cpp -L$(topdir) $(testlibs)

clean:
	rm $(objects)
