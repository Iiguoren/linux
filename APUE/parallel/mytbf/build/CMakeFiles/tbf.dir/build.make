# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.30

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = D:\cmake\bin\cmake.exe

# The command to remove a file.
RM = D:\cmake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "D:\vscode project\markdown\linux\DPUE\parall\mytbf"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "D:\vscode project\markdown\linux\DPUE\parall\mytbf\build"

# Include any dependencies generated for this target.
include CMakeFiles/tbf.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tbf.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tbf.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tbf.dir/flags.make

CMakeFiles/tbf.dir/main.c.obj: CMakeFiles/tbf.dir/flags.make
CMakeFiles/tbf.dir/main.c.obj: D:/vscode\ project/markdown/linux/DPUE/parall/mytbf/main.c
CMakeFiles/tbf.dir/main.c.obj: CMakeFiles/tbf.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="D:\vscode project\markdown\linux\DPUE\parall\mytbf\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/tbf.dir/main.c.obj"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/tbf.dir/main.c.obj -MF CMakeFiles\tbf.dir\main.c.obj.d -o CMakeFiles\tbf.dir\main.c.obj -c "D:\vscode project\markdown\linux\DPUE\parall\mytbf\main.c"

CMakeFiles/tbf.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/tbf.dir/main.c.i"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "D:\vscode project\markdown\linux\DPUE\parall\mytbf\main.c" > CMakeFiles\tbf.dir\main.c.i

CMakeFiles/tbf.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/tbf.dir/main.c.s"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "D:\vscode project\markdown\linux\DPUE\parall\mytbf\main.c" -o CMakeFiles\tbf.dir\main.c.s

# Object files for target tbf
tbf_OBJECTS = \
"CMakeFiles/tbf.dir/main.c.obj"

# External object files for target tbf
tbf_EXTERNAL_OBJECTS =

tbf.exe: CMakeFiles/tbf.dir/main.c.obj
tbf.exe: CMakeFiles/tbf.dir/build.make
tbf.exe: libmytbfLib.dll.a
tbf.exe: CMakeFiles/tbf.dir/linkLibs.rsp
tbf.exe: CMakeFiles/tbf.dir/objects1.rsp
tbf.exe: CMakeFiles/tbf.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="D:\vscode project\markdown\linux\DPUE\parall\mytbf\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable tbf.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\tbf.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tbf.dir/build: tbf.exe
.PHONY : CMakeFiles/tbf.dir/build

CMakeFiles/tbf.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\tbf.dir\cmake_clean.cmake
.PHONY : CMakeFiles/tbf.dir/clean

CMakeFiles/tbf.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "D:\vscode project\markdown\linux\DPUE\parall\mytbf" "D:\vscode project\markdown\linux\DPUE\parall\mytbf" "D:\vscode project\markdown\linux\DPUE\parall\mytbf\build" "D:\vscode project\markdown\linux\DPUE\parall\mytbf\build" "D:\vscode project\markdown\linux\DPUE\parall\mytbf\build\CMakeFiles\tbf.dir\DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/tbf.dir/depend

