# compile and link the library
file(GLOB SOURCES *.c)
add_library(rootio-core SHARED ${SOURCES})
target_link_libraries(rootio-core)

# various tests
add_subdirectory(tests)
