# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vk/software/rootio/c/src/rootio

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vk/software/rootio/c/src/rootio/core

# Include any dependencies generated for this target.
include core/tests/CMakeFiles/dump_records.dir/depend.make

# Include the progress variables for this target.
include core/tests/CMakeFiles/dump_records.dir/progress.make

# Include the compile flags for this target's objects.
include core/tests/CMakeFiles/dump_records.dir/flags.make

core/tests/CMakeFiles/dump_records.dir/dump_records.o: core/tests/CMakeFiles/dump_records.dir/flags.make
core/tests/CMakeFiles/dump_records.dir/dump_records.o: tests/dump_records.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/vk/software/rootio/c/src/rootio/core/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object core/tests/CMakeFiles/dump_records.dir/dump_records.o"
	cd /Users/vk/software/rootio/c/src/rootio/core/core/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dump_records.dir/dump_records.o   -c /Users/vk/software/rootio/c/src/rootio/core/tests/dump_records.c

core/tests/CMakeFiles/dump_records.dir/dump_records.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dump_records.dir/dump_records.i"
	cd /Users/vk/software/rootio/c/src/rootio/core/core/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/vk/software/rootio/c/src/rootio/core/tests/dump_records.c > CMakeFiles/dump_records.dir/dump_records.i

core/tests/CMakeFiles/dump_records.dir/dump_records.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dump_records.dir/dump_records.s"
	cd /Users/vk/software/rootio/c/src/rootio/core/core/tests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/vk/software/rootio/c/src/rootio/core/tests/dump_records.c -o CMakeFiles/dump_records.dir/dump_records.s

# Object files for target dump_records
dump_records_OBJECTS = \
"CMakeFiles/dump_records.dir/dump_records.o"

# External object files for target dump_records
dump_records_EXTERNAL_OBJECTS =

core/tests/dump_records: core/tests/CMakeFiles/dump_records.dir/dump_records.o
core/tests/dump_records: core/tests/CMakeFiles/dump_records.dir/build.make
core/tests/dump_records: core/librootio-core.dylib
core/tests/dump_records: core/tests/CMakeFiles/dump_records.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/vk/software/rootio/c/src/rootio/core/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable dump_records"
	cd /Users/vk/software/rootio/c/src/rootio/core/core/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dump_records.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
core/tests/CMakeFiles/dump_records.dir/build: core/tests/dump_records

.PHONY : core/tests/CMakeFiles/dump_records.dir/build

core/tests/CMakeFiles/dump_records.dir/clean:
	cd /Users/vk/software/rootio/c/src/rootio/core/core/tests && $(CMAKE_COMMAND) -P CMakeFiles/dump_records.dir/cmake_clean.cmake
.PHONY : core/tests/CMakeFiles/dump_records.dir/clean

core/tests/CMakeFiles/dump_records.dir/depend:
	cd /Users/vk/software/rootio/c/src/rootio/core && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vk/software/rootio/c/src/rootio /Users/vk/software/rootio/c/src/rootio/core/tests /Users/vk/software/rootio/c/src/rootio/core /Users/vk/software/rootio/c/src/rootio/core/core/tests /Users/vk/software/rootio/c/src/rootio/core/core/tests/CMakeFiles/dump_records.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : core/tests/CMakeFiles/dump_records.dir/depend

