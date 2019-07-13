cmake_minimum_required(VERSION 3.8)

# keep project version in here for now
project (XXX VERSION 0.0.0)

# for c++ add the right flags for the standard
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")

# search module path
#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# inclusion
include_directories(${PROJECT_SOURCE_DIR}/src)

# traverse all the components
add_subdirectory(src/XXX)
