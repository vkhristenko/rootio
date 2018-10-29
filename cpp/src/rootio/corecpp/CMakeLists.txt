# for c++ add the right flags for the standard
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")

# compile and link the library
file(GLOB SOURCES *.cpp)
add_library(rootio-corecpp SHARED ${SOURCES})
target_link_libraries(rootio-corecpp rootio-core)

# various simple tests
add_subdirectory(tests)

# tests with ROOT
find_package(ROOT REQUIRED RIO)
if (ROOT_FOUND)
    include_directories("${ROOT_INCLUDE_DIRS}")
    add_subdirectory(root-tests)
endif(ROOT_FOUND)

# install generated artifacts (lib + headers)
file(GLOB HEADERS *.hpp)
install(TARGETS rootio-corecpp 
        LIBRARY DESTINATION lib)
install(
    FILES ${HEADERS}
    DESTINATION include/rootio/corecpp)
