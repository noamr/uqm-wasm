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

# Utility rule file for pnglibconf_h.

# Include any custom commands dependencies for this target.
include CMakeFiles/pnglibconf_h.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/pnglibconf_h.dir/progress.make

CMakeFiles/pnglibconf_h: pnglibconf.h

pnglibconf.h: pnglibconf.out
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/nrosenthal/code/libpng-1.6.40/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating pnglibconf.h"
	/opt/homebrew/Cellar/cmake/3.27.7/bin/cmake -DOUTPUT=pnglibconf.h -P /Users/nrosenthal/code/libpng-1.6.40/scripts/gensrc.cmake

pnglibconf.out: pnglibconf.c
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/nrosenthal/code/libpng-1.6.40/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating pnglibconf.out"
	/opt/homebrew/Cellar/cmake/3.27.7/bin/cmake -DINPUT=/Users/nrosenthal/code/libpng-1.6.40/pnglibconf.c -DOUTPUT=/Users/nrosenthal/code/libpng-1.6.40/pnglibconf.out -P /Users/nrosenthal/code/libpng-1.6.40/scripts/genout.cmake

pnglibconf.c: scripts/pnglibconf.dfa
pnglibconf.c: scripts/options.awk
pnglibconf.c: pngconf.h
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/nrosenthal/code/libpng-1.6.40/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Generating pnglibconf.c"
	/opt/homebrew/Cellar/cmake/3.27.7/bin/cmake -DOUTPUT=pnglibconf.c -P /Users/nrosenthal/code/libpng-1.6.40/scripts/gensrc.cmake

pnglibconf_h: CMakeFiles/pnglibconf_h
pnglibconf_h: pnglibconf.c
pnglibconf_h: pnglibconf.h
pnglibconf_h: pnglibconf.out
pnglibconf_h: CMakeFiles/pnglibconf_h.dir/build.make
.PHONY : pnglibconf_h

# Rule to build all files generated by this target.
CMakeFiles/pnglibconf_h.dir/build: pnglibconf_h
.PHONY : CMakeFiles/pnglibconf_h.dir/build

CMakeFiles/pnglibconf_h.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pnglibconf_h.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pnglibconf_h.dir/clean

CMakeFiles/pnglibconf_h.dir/depend:
	cd /Users/nrosenthal/code/libpng-1.6.40 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40/CMakeFiles/pnglibconf_h.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/pnglibconf_h.dir/depend

