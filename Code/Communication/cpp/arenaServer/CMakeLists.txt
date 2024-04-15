cmake_minimum_required(VERSION 3.8)
project(arenaServer)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
add_executable(test src/test.cpp)
#target_link_libraries(arenaServer socket)
#target_include_directories(arenaServer PRIVATE ${PROTOBUF_INCLUDE_DIRS})
#target_link_libraries(arenaServer PRIVATE ${PROTOBUF_LIBRARIES})

install(TARGETS
		test
		DESTINATION lib/${PROJECT_NAME})
