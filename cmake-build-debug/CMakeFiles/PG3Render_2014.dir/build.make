# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /snap/clion/112/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/112/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sebastian/Programs/PG3Render-2014

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sebastian/Programs/PG3Render-2014/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/PG3Render_2014.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/PG3Render_2014.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PG3Render_2014.dir/flags.make

CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.o: CMakeFiles/PG3Render_2014.dir/flags.make
CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.o: ../src/pg3render.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sebastian/Programs/PG3Render-2014/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.o"
	/usr/bin/clang++-9  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.o -c /home/sebastian/Programs/PG3Render-2014/src/pg3render.cxx

CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.i"
	/usr/bin/clang++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sebastian/Programs/PG3Render-2014/src/pg3render.cxx > CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.i

CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.s"
	/usr/bin/clang++-9 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sebastian/Programs/PG3Render-2014/src/pg3render.cxx -o CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.s

# Object files for target PG3Render_2014
PG3Render_2014_OBJECTS = \
"CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.o"

# External object files for target PG3Render_2014
PG3Render_2014_EXTERNAL_OBJECTS =

PG3Render_2014: CMakeFiles/PG3Render_2014.dir/src/pg3render.cxx.o
PG3Render_2014: CMakeFiles/PG3Render_2014.dir/build.make
PG3Render_2014: /usr/lib/llvm-9/lib/libomp.so
PG3Render_2014: /usr/lib/x86_64-linux-gnu/libpthread.so
PG3Render_2014: CMakeFiles/PG3Render_2014.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sebastian/Programs/PG3Render-2014/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable PG3Render_2014"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PG3Render_2014.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PG3Render_2014.dir/build: PG3Render_2014

.PHONY : CMakeFiles/PG3Render_2014.dir/build

CMakeFiles/PG3Render_2014.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PG3Render_2014.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PG3Render_2014.dir/clean

CMakeFiles/PG3Render_2014.dir/depend:
	cd /home/sebastian/Programs/PG3Render-2014/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sebastian/Programs/PG3Render-2014 /home/sebastian/Programs/PG3Render-2014 /home/sebastian/Programs/PG3Render-2014/cmake-build-debug /home/sebastian/Programs/PG3Render-2014/cmake-build-debug /home/sebastian/Programs/PG3Render-2014/cmake-build-debug/CMakeFiles/PG3Render_2014.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PG3Render_2014.dir/depend

