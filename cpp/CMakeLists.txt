cmake_minimum_required(VERSION 3.8)

# keep project version in here for now
project (rootio VERSION 0.0.0)

# search module path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# inclusion
include_directories(${PROJECT_SOURCE_DIR}/src)

# traverse all the components
add_subdirectory(src/rootio)
