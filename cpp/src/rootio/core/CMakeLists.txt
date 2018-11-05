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

# hdfs 
find_package(HDFS)
if (HDFS_FOUND)
    message ("found hdfs. setting up...")
    include_directories(${HDFS_INCLUDE_DIR})
    add_subdirectory(hdfs)
else(HDFS_FOUND)
    message ("hdfs was not found. do not include into the build...")
endif(HDFS_FOUND)
