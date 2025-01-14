cmake_minimum_required(VERSION 3.8)
project(droneClient)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#uncomment for debugging:
set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS "-g")

find_package(Protobuf 3.12.4 REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${HOME}/.local/include)

add_executable(droneClient src/droneMain.cpp src/clientImplementation.cpp src/threadFuncs.cpp src/IMUimplementation.cpp src/dronePosVec.pb.cc)
target_link_libraries(droneClient pigpio)

target_link_libraries(droneClient protobuf::libprotobuf protobuf::libprotobuf-lite)

install(TARGETS
		droneClient
		DESTINATION lib/${PROJECT_NAME})
