# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.30.4/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.30.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/prompt/Downloads/libssh-0.11.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/prompt/Downloads/libssh-0.11.0/build

# Include any dependencies generated for this target.
include tests/unittests/CMakeFiles/torture_bytearray.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/unittests/CMakeFiles/torture_bytearray.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/unittests/CMakeFiles/torture_bytearray.dir/progress.make

# Include the compile flags for this target's objects.
include tests/unittests/CMakeFiles/torture_bytearray.dir/flags.make

tests/unittests/CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o: tests/unittests/CMakeFiles/torture_bytearray.dir/flags.make
tests/unittests/CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o: /Users/prompt/Downloads/libssh-0.11.0/tests/unittests/torture_bytearray.c
tests/unittests/CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o: tests/unittests/CMakeFiles/torture_bytearray.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/prompt/Downloads/libssh-0.11.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/unittests/CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/tests/unittests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT tests/unittests/CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o -MF CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o.d -o CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o -c /Users/prompt/Downloads/libssh-0.11.0/tests/unittests/torture_bytearray.c

tests/unittests/CMakeFiles/torture_bytearray.dir/torture_bytearray.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/torture_bytearray.dir/torture_bytearray.c.i"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/tests/unittests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/prompt/Downloads/libssh-0.11.0/tests/unittests/torture_bytearray.c > CMakeFiles/torture_bytearray.dir/torture_bytearray.c.i

tests/unittests/CMakeFiles/torture_bytearray.dir/torture_bytearray.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/torture_bytearray.dir/torture_bytearray.c.s"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/tests/unittests && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/prompt/Downloads/libssh-0.11.0/tests/unittests/torture_bytearray.c -o CMakeFiles/torture_bytearray.dir/torture_bytearray.c.s

# Object files for target torture_bytearray
torture_bytearray_OBJECTS = \
"CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o"

# External object files for target torture_bytearray
torture_bytearray_EXTERNAL_OBJECTS =

tests/unittests/torture_bytearray: tests/unittests/CMakeFiles/torture_bytearray.dir/torture_bytearray.c.o
tests/unittests/torture_bytearray: tests/unittests/CMakeFiles/torture_bytearray.dir/build.make
tests/unittests/torture_bytearray: tests/libtorture.a
tests/unittests/torture_bytearray: /usr/local/lib/libcmocka.dylib
tests/unittests/torture_bytearray: src/libssh.a
tests/unittests/torture_bytearray: /opt/homebrew/lib/libcrypto.dylib
tests/unittests/torture_bytearray: /Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk/usr/lib/libz.tbd
tests/unittests/torture_bytearray: /Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk/usr/lib/libgssapi_krb5.tbd
tests/unittests/torture_bytearray: /Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk/usr/lib/libkrb5.tbd
tests/unittests/torture_bytearray: /Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk/usr/lib/libk5crypto.tbd
tests/unittests/torture_bytearray: /Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk/usr/lib/libcom_err.tbd
tests/unittests/torture_bytearray: tests/unittests/CMakeFiles/torture_bytearray.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/prompt/Downloads/libssh-0.11.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable torture_bytearray"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/tests/unittests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/torture_bytearray.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/unittests/CMakeFiles/torture_bytearray.dir/build: tests/unittests/torture_bytearray
.PHONY : tests/unittests/CMakeFiles/torture_bytearray.dir/build

tests/unittests/CMakeFiles/torture_bytearray.dir/clean:
	cd /Users/prompt/Downloads/libssh-0.11.0/build/tests/unittests && $(CMAKE_COMMAND) -P CMakeFiles/torture_bytearray.dir/cmake_clean.cmake
.PHONY : tests/unittests/CMakeFiles/torture_bytearray.dir/clean

tests/unittests/CMakeFiles/torture_bytearray.dir/depend:
	cd /Users/prompt/Downloads/libssh-0.11.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/prompt/Downloads/libssh-0.11.0 /Users/prompt/Downloads/libssh-0.11.0/tests/unittests /Users/prompt/Downloads/libssh-0.11.0/build /Users/prompt/Downloads/libssh-0.11.0/build/tests/unittests /Users/prompt/Downloads/libssh-0.11.0/build/tests/unittests/CMakeFiles/torture_bytearray.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : tests/unittests/CMakeFiles/torture_bytearray.dir/depend

