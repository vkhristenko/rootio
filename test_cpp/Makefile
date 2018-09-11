sources = $(wildcard *.cpp)
objects = $(sources:.cpp=)
cxx = clang++
cxxflags = -fPIC -Wall -Wextra -O2 -g --std=c++17
testlibs = -l$(libname) $(libs)

.PHONY: clean

all: $(objects)

.cpp:
	$(cxx) $(cxxflags) $(includedirs) -o $@ $@.cpp -L$(topdir) $(testlibs)

clean:
	rm $(objects)
