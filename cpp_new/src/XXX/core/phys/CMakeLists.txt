# compile and link the library
file(GLOB SOURCES *.cpp)
add_library(XXX-core-phys SHARED ${SOURCES})
target_link_libraries(XXX-core-phys XXX-core-common)

# compile tests as well
add_subdirectory(tests)
