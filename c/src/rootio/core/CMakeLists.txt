# compile and link the library
file(GLOB SOURCES *.c)
add_library(rootio-core SHARED ${SOURCES})
target_link_libraries(rootio-core)

# various tests
add_subdirectory(tests)

# install generated artifacts (lib + headers)
file(GLOB HEADERS *.h)
install(TARGETS rootio-core 
        LIBRARY DESTINATION lib)
install(
    FILES ${HEADERS}
    DESTINATION include/rootio/core)
