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
include examples/CMakeFiles/senddata.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include examples/CMakeFiles/senddata.dir/compiler_depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/senddata.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/senddata.dir/flags.make

examples/CMakeFiles/senddata.dir/senddata.c.o: examples/CMakeFiles/senddata.dir/flags.make
examples/CMakeFiles/senddata.dir/senddata.c.o: /Users/prompt/Downloads/libssh-0.11.0/examples/senddata.c
examples/CMakeFiles/senddata.dir/senddata.c.o: examples/CMakeFiles/senddata.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/prompt/Downloads/libssh-0.11.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object examples/CMakeFiles/senddata.dir/senddata.c.o"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT examples/CMakeFiles/senddata.dir/senddata.c.o -MF CMakeFiles/senddata.dir/senddata.c.o.d -o CMakeFiles/senddata.dir/senddata.c.o -c /Users/prompt/Downloads/libssh-0.11.0/examples/senddata.c

examples/CMakeFiles/senddata.dir/senddata.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/senddata.dir/senddata.c.i"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/prompt/Downloads/libssh-0.11.0/examples/senddata.c > CMakeFiles/senddata.dir/senddata.c.i

examples/CMakeFiles/senddata.dir/senddata.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/senddata.dir/senddata.c.s"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/prompt/Downloads/libssh-0.11.0/examples/senddata.c -o CMakeFiles/senddata.dir/senddata.c.s

examples/CMakeFiles/senddata.dir/authentication.c.o: examples/CMakeFiles/senddata.dir/flags.make
examples/CMakeFiles/senddata.dir/authentication.c.o: /Users/prompt/Downloads/libssh-0.11.0/examples/authentication.c
examples/CMakeFiles/senddata.dir/authentication.c.o: examples/CMakeFiles/senddata.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/prompt/Downloads/libssh-0.11.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object examples/CMakeFiles/senddata.dir/authentication.c.o"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT examples/CMakeFiles/senddata.dir/authentication.c.o -MF CMakeFiles/senddata.dir/authentication.c.o.d -o CMakeFiles/senddata.dir/authentication.c.o -c /Users/prompt/Downloads/libssh-0.11.0/examples/authentication.c

examples/CMakeFiles/senddata.dir/authentication.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/senddata.dir/authentication.c.i"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/prompt/Downloads/libssh-0.11.0/examples/authentication.c > CMakeFiles/senddata.dir/authentication.c.i

examples/CMakeFiles/senddata.dir/authentication.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/senddata.dir/authentication.c.s"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/prompt/Downloads/libssh-0.11.0/examples/authentication.c -o CMakeFiles/senddata.dir/authentication.c.s

examples/CMakeFiles/senddata.dir/knownhosts.c.o: examples/CMakeFiles/senddata.dir/flags.make
examples/CMakeFiles/senddata.dir/knownhosts.c.o: /Users/prompt/Downloads/libssh-0.11.0/examples/knownhosts.c
examples/CMakeFiles/senddata.dir/knownhosts.c.o: examples/CMakeFiles/senddata.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/prompt/Downloads/libssh-0.11.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object examples/CMakeFiles/senddata.dir/knownhosts.c.o"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT examples/CMakeFiles/senddata.dir/knownhosts.c.o -MF CMakeFiles/senddata.dir/knownhosts.c.o.d -o CMakeFiles/senddata.dir/knownhosts.c.o -c /Users/prompt/Downloads/libssh-0.11.0/examples/knownhosts.c

examples/CMakeFiles/senddata.dir/knownhosts.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/senddata.dir/knownhosts.c.i"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/prompt/Downloads/libssh-0.11.0/examples/knownhosts.c > CMakeFiles/senddata.dir/knownhosts.c.i

examples/CMakeFiles/senddata.dir/knownhosts.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/senddata.dir/knownhosts.c.s"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/prompt/Downloads/libssh-0.11.0/examples/knownhosts.c -o CMakeFiles/senddata.dir/knownhosts.c.s

examples/CMakeFiles/senddata.dir/connect_ssh.c.o: examples/CMakeFiles/senddata.dir/flags.make
examples/CMakeFiles/senddata.dir/connect_ssh.c.o: /Users/prompt/Downloads/libssh-0.11.0/examples/connect_ssh.c
examples/CMakeFiles/senddata.dir/connect_ssh.c.o: examples/CMakeFiles/senddata.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/prompt/Downloads/libssh-0.11.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object examples/CMakeFiles/senddata.dir/connect_ssh.c.o"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT examples/CMakeFiles/senddata.dir/connect_ssh.c.o -MF CMakeFiles/senddata.dir/connect_ssh.c.o.d -o CMakeFiles/senddata.dir/connect_ssh.c.o -c /Users/prompt/Downloads/libssh-0.11.0/examples/connect_ssh.c

examples/CMakeFiles/senddata.dir/connect_ssh.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/senddata.dir/connect_ssh.c.i"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/prompt/Downloads/libssh-0.11.0/examples/connect_ssh.c > CMakeFiles/senddata.dir/connect_ssh.c.i

examples/CMakeFiles/senddata.dir/connect_ssh.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/senddata.dir/connect_ssh.c.s"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/prompt/Downloads/libssh-0.11.0/examples/connect_ssh.c -o CMakeFiles/senddata.dir/connect_ssh.c.s

# Object files for target senddata
senddata_OBJECTS = \
"CMakeFiles/senddata.dir/senddata.c.o" \
"CMakeFiles/senddata.dir/authentication.c.o" \
"CMakeFiles/senddata.dir/knownhosts.c.o" \
"CMakeFiles/senddata.dir/connect_ssh.c.o"

# External object files for target senddata
senddata_EXTERNAL_OBJECTS =

examples/senddata: examples/CMakeFiles/senddata.dir/senddata.c.o
examples/senddata: examples/CMakeFiles/senddata.dir/authentication.c.o
examples/senddata: examples/CMakeFiles/senddata.dir/knownhosts.c.o
examples/senddata: examples/CMakeFiles/senddata.dir/connect_ssh.c.o
examples/senddata: examples/CMakeFiles/senddata.dir/build.make
examples/senddata: lib/libssh.4.10.0.dylib
examples/senddata: examples/CMakeFiles/senddata.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/prompt/Downloads/libssh-0.11.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable senddata"
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/senddata.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/senddata.dir/build: examples/senddata
.PHONY : examples/CMakeFiles/senddata.dir/build

examples/CMakeFiles/senddata.dir/clean:
	cd /Users/prompt/Downloads/libssh-0.11.0/build/examples && $(CMAKE_COMMAND) -P CMakeFiles/senddata.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/senddata.dir/clean

examples/CMakeFiles/senddata.dir/depend:
	cd /Users/prompt/Downloads/libssh-0.11.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/prompt/Downloads/libssh-0.11.0 /Users/prompt/Downloads/libssh-0.11.0/examples /Users/prompt/Downloads/libssh-0.11.0/build /Users/prompt/Downloads/libssh-0.11.0/build/examples /Users/prompt/Downloads/libssh-0.11.0/build/examples/CMakeFiles/senddata.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : examples/CMakeFiles/senddata.dir/depend

