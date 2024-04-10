cmake_minimum_required(VERSION 3.8)
project(droneClient)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(droneClient src/droneClient.cpp)

install(TARGETS
		droneClient
		DESTINATION lib/${PROJECT_NAME})
