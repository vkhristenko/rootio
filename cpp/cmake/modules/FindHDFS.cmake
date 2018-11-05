exec_program(hadoop 
    ARGS version 
    OUTPUT_VARIABLE Hadoop_VERSION
    RETURN_VALUE Hadoop_RETURN)

set(HADOOP_INCLUDE_DIR $ENV{HADOOP_HOME}/include)
set(HADOOP_LIB_DIR $ENV{HADOOP_HOME}/lib)
#message ("HADOOP_INCLUDE_DIR = ${HADOOP_INCLUDE_DIR}")
#message ("HADOOP_LIB_DIR = ${HADOOP_LIB_DIR}")

# currently only looking in HADOOP_HOME
find_path(HDFS_INCLUDE_DIR hdfs.h PATHS
  $ENV{HADOOP_HOME}/include/
  # make sure we don't accidentally pick up a different version
  NO_DEFAULT_PATH
)

if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
  set(arch_hint "x64")
elseif ("$ENV{LIB}" MATCHES "(amd64|ia64)")
  set(arch_hint "x64")
else ()
  set(arch_hint "x86")
endif()

message(STATUS "Architecture: ${arch_hint}")

if ("${arch_hint}" STREQUAL "x64")
  set(HDFS_LIB_PATHS $ENV{HADOOP_HOME}/lib/native)
else ()
  set(HDFS_LIB_PATHS $ENV{HADOOP_HOME}/lib/native)
endif ()

message(STATUS "HDFS_LIB_PATHS: ${HDFS_LIB_PATHS}")

find_library(HDFS_LIB NAMES hdfs PATHS
  ${HDFS_LIB_PATHS}
  # make sure we don't accidentally pick up a different version
  NO_DEFAULT_PATH
)

if (HDFS_LIB)
  set(HDFS_FOUND TRUE)
  set(HDFS_LIBRARIES ${HDFS_LIB})
  set(HDFS_STATIC_LIB ${HDFS_LIB_PATHS}/${CMAKE_STATIC_LIBRARY_PREFIX}hdfs${CMAKE_STATIC_LIBRARY_SUFFIX})

  add_library(hdfs_static STATIC IMPORTED)
  set_target_properties(hdfs_static PROPERTIES IMPORTED_LOCATION ${HDFS_STATIC_LIB})

else ()
  set(HDFS_FOUND FALSE)
endif ()

if (HDFS_FOUND)
  if (NOT HDFS_FIND_QUIETLY)
    message(STATUS "${Hadoop_VERSION}")
    message(STATUS "HDFS_INCLUDE_DIR: ${HDFS_INCLUDE_DIR}")
    message(STATUS "HDFS_LIBRARIES: ${HDFS_LIBRARIES}")
    message(STATUS "hdfs_static: ${HDFS_STATIC_LIB}")
  endif ()
else ()
  message(FATAL_ERROR "HDFS includes and libraries NOT found."
    "(${HDFS_INCLUDE_DIR}, ${HDFS_LIB})")
endif ()

mark_as_advanced(
  HDFS_LIBRARIES
  HDFS_INCLUDE_DIR
  hdfs_static
)
