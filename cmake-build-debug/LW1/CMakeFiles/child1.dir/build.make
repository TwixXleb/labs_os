# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.16

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.1\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\Projects\CLionProjects\labs_os

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\Projects\CLionProjects\labs_os\cmake-build-debug

# Include any dependencies generated for this target.
include LW1/CMakeFiles/child1.dir/depend.make

# Include the progress variables for this target.
include LW1/CMakeFiles/child1.dir/progress.make

# Include the compile flags for this target's objects.
include LW1/CMakeFiles/child1.dir/flags.make

LW1/CMakeFiles/child1.dir/src/child1.c.obj: LW1/CMakeFiles/child1.dir/flags.make
LW1/CMakeFiles/child1.dir/src/child1.c.obj: LW1/CMakeFiles/child1.dir/includes_C.rsp
LW1/CMakeFiles/child1.dir/src/child1.c.obj: ../LW1/src/child1.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Projects\CLionProjects\labs_os\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object LW1/CMakeFiles/child1.dir/src/child1.c.obj"
	cd /d D:\Projects\CLionProjects\labs_os\cmake-build-debug\LW1 && C:\Users\dshir\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\child1.dir\src\child1.c.obj   -c D:\Projects\CLionProjects\labs_os\LW1\src\child1.c

LW1/CMakeFiles/child1.dir/src/child1.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/child1.dir/src/child1.c.i"
	cd /d D:\Projects\CLionProjects\labs_os\cmake-build-debug\LW1 && C:\Users\dshir\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Projects\CLionProjects\labs_os\LW1\src\child1.c > CMakeFiles\child1.dir\src\child1.c.i

LW1/CMakeFiles/child1.dir/src/child1.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/child1.dir/src/child1.c.s"
	cd /d D:\Projects\CLionProjects\labs_os\cmake-build-debug\LW1 && C:\Users\dshir\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\Projects\CLionProjects\labs_os\LW1\src\child1.c -o CMakeFiles\child1.dir\src\child1.c.s

# Object files for target child1
child1_OBJECTS = \
"CMakeFiles/child1.dir/src/child1.c.obj"

# External object files for target child1
child1_EXTERNAL_OBJECTS =

LW1/child1.exe: LW1/CMakeFiles/child1.dir/src/child1.c.obj
LW1/child1.exe: LW1/CMakeFiles/child1.dir/build.make
LW1/child1.exe: LW1/libutils.a
LW1/child1.exe: LW1/CMakeFiles/child1.dir/linklibs.rsp
LW1/child1.exe: LW1/CMakeFiles/child1.dir/objects1.rsp
LW1/child1.exe: LW1/CMakeFiles/child1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\Projects\CLionProjects\labs_os\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable child1.exe"
	cd /d D:\Projects\CLionProjects\labs_os\cmake-build-debug\LW1 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\child1.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
LW1/CMakeFiles/child1.dir/build: LW1/child1.exe

.PHONY : LW1/CMakeFiles/child1.dir/build

LW1/CMakeFiles/child1.dir/clean:
	cd /d D:\Projects\CLionProjects\labs_os\cmake-build-debug\LW1 && $(CMAKE_COMMAND) -P CMakeFiles\child1.dir\cmake_clean.cmake
.PHONY : LW1/CMakeFiles/child1.dir/clean

LW1/CMakeFiles/child1.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\Projects\CLionProjects\labs_os D:\Projects\CLionProjects\labs_os\LW1 D:\Projects\CLionProjects\labs_os\cmake-build-debug D:\Projects\CLionProjects\labs_os\cmake-build-debug\LW1 D:\Projects\CLionProjects\labs_os\cmake-build-debug\LW1\CMakeFiles\child1.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : LW1/CMakeFiles/child1.dir/depend
