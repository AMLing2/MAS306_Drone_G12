cmake_minimum_required(VERSION 3.8)
project(arenaServer)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(arenaServer src/arenaServer.cpp)
#target_link_libraries(arenaServer socket)

install(TARGETS
		arenaServer
		DESTINATION lib/${PROJECT_NAME})
