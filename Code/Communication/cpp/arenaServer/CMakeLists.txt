cmake_minimum_required(VERSION 3.8)
project(arenaServer)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


#uncomment for debugging:
set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS "-g")

find_package(Protobuf 3.12.4 REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${HOME}/.local/include)

add_executable(test src/test.cpp src/dronePosVec.pb.cc)
add_executable(arenaServer src/messengerClass.cpp src/msgThreads.cpp src/serverMain.cpp src/dronePosVec.pb.cc)

target_link_libraries(test protobuf::libprotobuf protobuf::libprotobuf-lite)
target_link_libraries(arenaServer protobuf::libprotobuf protobuf::libprotobuf-lite)

#target_include_directories(test PRIVATE ${HOME}/.local/include)
#target_link_libraries(test PRIVATE ${HOME}/.local/lib/libprotobuf.so)

install(TARGETS
		arenaServer
		test
		DESTINATION lib/${PROJECT_NAME})
