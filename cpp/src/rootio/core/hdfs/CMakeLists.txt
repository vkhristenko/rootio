# compile and link the library
file(GLOB SOURCES *.c)
add_library(rootio-core-hdfs SHARED ${SOURCES})
target_link_libraries(rootio-core-hdfs rootio-core ${HDFS_LIBRARIES})

# various tests
add_subdirectory(tests)

# install generated artifacts (lib + headers)
file(GLOB HEADERS *.h)
install(TARGETS rootio-core-hdfs
        LIBRARY DESTINATION lib)
install(
    FILES ${HEADERS}
    DESTINATION include/rootio/core/hdfs)
