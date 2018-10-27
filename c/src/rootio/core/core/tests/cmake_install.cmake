# Install script for directory: /Users/vk/software/rootio/c/src/rootio/core/tests

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/vk/software/rootio/c/build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/tests/core" TYPE EXECUTABLE FILES "/Users/vk/software/rootio/c/src/rootio/core/core/tests/dump_dirstructure_recursively")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/dump_dirstructure_recursively" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/dump_dirstructure_recursively")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/vk/software/rootio/c/src/rootio/core/core/librootio-core.dylib" "librootio-core.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/dump_dirstructure_recursively")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/dump_dirstructure_recursively")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/tests/core" TYPE EXECUTABLE FILES "/Users/vk/software/rootio/c/src/rootio/core/core/tests/dump_records")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/dump_records" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/dump_records")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/vk/software/rootio/c/src/rootio/core/core/librootio-core.dylib" "librootio-core.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/dump_records")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/dump_records")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/tests/core" TYPE EXECUTABLE FILES "/Users/vk/software/rootio/c/src/rootio/core/core/tests/test1")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/test1" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/test1")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/vk/software/rootio/c/src/rootio/core/core/librootio-core.dylib" "librootio-core.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/test1")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/test1")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/tests/core" TYPE EXECUTABLE FILES "/Users/vk/software/rootio/c/src/rootio/core/core/tests/test_write")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/test_write" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/test_write")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/vk/software/rootio/c/src/rootio/core/core/librootio-core.dylib" "librootio-core.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/test_write")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/tests/core/test_write")
    endif()
  endif()
endif()

