# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/build

# Include any dependencies generated for this target.
include CMakeFiles/dronePosVec.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/dronePosVec.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/dronePosVec.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dronePosVec.dir/flags.make

CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o: CMakeFiles/dronePosVec.dir/flags.make
CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o: ../src/dronePosVec.pb.cc
CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o: CMakeFiles/dronePosVec.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o -MF CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o.d -o CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o -c /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/src/dronePosVec.pb.cc

CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/src/dronePosVec.pb.cc > CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.i

CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/src/dronePosVec.pb.cc -o CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.s

# Object files for target dronePosVec
dronePosVec_OBJECTS = \
"CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o"

# External object files for target dronePosVec
dronePosVec_EXTERNAL_OBJECTS =

dronePosVec: CMakeFiles/dronePosVec.dir/src/dronePosVec.pb.cc.o
dronePosVec: CMakeFiles/dronePosVec.dir/build.make
dronePosVec: /usr/lib/x86_64-linux-gnu/libprotobuf.so
dronePosVec: /usr/lib/x86_64-linux-gnu/libprotobuf-lite.so
dronePosVec: CMakeFiles/dronePosVec.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable dronePosVec"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dronePosVec.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dronePosVec.dir/build: dronePosVec
.PHONY : CMakeFiles/dronePosVec.dir/build

CMakeFiles/dronePosVec.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dronePosVec.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dronePosVec.dir/clean

CMakeFiles/dronePosVec.dir/depend:
	cd /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/build /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/build /home/adrian/MAS306_Drone_G12/Code/Communication/cpp/arenaServer/build/CMakeFiles/dronePosVec.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dronePosVec.dir/depend

