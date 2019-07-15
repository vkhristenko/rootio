# compile and link the library
file(GLOB SOURCES *.cpp)
add_library(XXX-core-baselogic SHARED ${SOURCES})
target_link_libraries(XXX-core-baselogic XXX-core-common XXX-core-phys)

# compile tests as well
add_subdirectory(tests)
