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

# Utility rule file for png_scripts_pnglibconf_c.

# Include any custom commands dependencies for this target.
include CMakeFiles/png_scripts_pnglibconf_c.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/png_scripts_pnglibconf_c.dir/progress.make

CMakeFiles/png_scripts_pnglibconf_c: scripts/pnglibconf.c

scripts/pnglibconf.c: scripts/pnglibconf.dfa
scripts/pnglibconf.c: scripts/options.awk
scripts/pnglibconf.c: pngconf.h
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/Users/nrosenthal/code/libpng-1.6.40/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating scripts/pnglibconf.c"
	/opt/homebrew/Cellar/cmake/3.27.7/bin/cmake -DOUTPUT=scripts/pnglibconf.c -P /Users/nrosenthal/code/libpng-1.6.40/scripts/gensrc.cmake

png_scripts_pnglibconf_c: CMakeFiles/png_scripts_pnglibconf_c
png_scripts_pnglibconf_c: scripts/pnglibconf.c
png_scripts_pnglibconf_c: CMakeFiles/png_scripts_pnglibconf_c.dir/build.make
.PHONY : png_scripts_pnglibconf_c

# Rule to build all files generated by this target.
CMakeFiles/png_scripts_pnglibconf_c.dir/build: png_scripts_pnglibconf_c
.PHONY : CMakeFiles/png_scripts_pnglibconf_c.dir/build

CMakeFiles/png_scripts_pnglibconf_c.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/png_scripts_pnglibconf_c.dir/cmake_clean.cmake
.PHONY : CMakeFiles/png_scripts_pnglibconf_c.dir/clean

CMakeFiles/png_scripts_pnglibconf_c.dir/depend:
	cd /Users/nrosenthal/code/libpng-1.6.40 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40 /Users/nrosenthal/code/libpng-1.6.40/CMakeFiles/png_scripts_pnglibconf_c.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/png_scripts_pnglibconf_c.dir/depend

