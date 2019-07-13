# compile and link the library
file(GLOB SOURCES *.cpp)
add_library(XXX-core-common SHARED ${SOURCES})
target_link_libraries(XXX-core-common)
