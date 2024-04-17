cmake_minimum_required(VERSION 3.8)
project(droneClient)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Protobuf 3.12.4 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${HOME}/.local/include)

add_executable(droneClient src/droneClient.cpp src/dronePosVec.pb.cc)

target_link_libraries(droneClient protobuf::libprotobuf protobuf::libprotobuf-lite)

install(TARGETS
		droneClient
		DESTINATION lib/${PROJECT_NAME})
