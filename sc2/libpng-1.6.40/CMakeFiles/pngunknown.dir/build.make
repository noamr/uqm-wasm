# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.27.7/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.27.7/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/nrosenthal/code/libpng-1.6.40

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/nrosenthal/code/libpng-1.6.40

# Include any dependencies generated for this target.
include CMakeFiles/pngunknown.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/pngunknown.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/pngunknown.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pngunknown.dir/flags.make

CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o: CMakeFiles/pngunknown.dir/flags.make
CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o: CMakeFiles/pngunknown.dir/includes_C.rsp
CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o: contrib/libtests/pngunknown.c
CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o: CMakeFiles/pngunknown.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/nrosenthal/code/libpng-1.6.40/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o"
	/Users/nrosenthal/code/emsdk/upstream/emscripten/emcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o -MF CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o.d -o CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o -c /Users/nrosenthal/code/libpng-1.6.40/contrib/libtests/pngunknown.c

CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.i"
	/Users/nrosenthal/code/emsdk/upstream/emscripten/emcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/nrosenthal/code/libpng-1.6.40/contrib/libtests/pngunknown.c > CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.i

CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.s"
	/Users/nrosenthal/code/emsdk/upstream/emscripten/emcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/nrosenthal/code/libpng-1.6.40/contrib/libtests/pngunknown.c -o CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.s

# Object files for target pngunknown
pngunknown_OBJECTS = \
"CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o"

# External object files for target pngunknown
pngunknown_EXTERNAL_OBJECTS =

pngunknown.js: CMakeFiles/pngunknown.dir/contrib/libtests/pngunknown.c.o
pngunknown.js: CMakeFiles/pngunknown.dir/build.make
pngunknown.js: libpng16.a
pngunknown.js: /Users/nrosenthal/code/emsdk/upstream/emscripten/cache/sysroot/lib/wasm32-emscripten/libz.a
pngunknown.js: CMakeFiles/pngunknown.dir/linkLibs.rsp
pngunknown.js: CMakeFiles/pngunknown.dir/objects1.rsp
pngunknown.js: CMakeFiles/pngunknown.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/nrosenthal/code/libpng-1.6.40/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable pngunknown.js"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pngunknown.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/pngunknown.dir/build: pngunknown.js
.PHONY : CMakeFiles/pngunknown.dir/build

CMakeFiles/pngunknown.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pngunknown.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pngunknown.dir/clean

CMakeFiles/pngunknown.dir/depend:
	cd /Users/nrosenthal/code/libpng-1.6.40 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40/CMakeFiles/pngunknown.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/pngunknown.dir/depend

