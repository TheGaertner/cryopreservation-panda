cmake_minimum_required(VERSION 3.4)

# set(CXX "/usr/bin/g++-8")

project(panda-controller)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/../cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Franka REQUIRED)
find_package(Eigen3 REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(yaml-cpp REQUIRED )
find_package(Boost REQUIRED COMPONENTS system serialization)

add_definitions     ( ${EIGEN3_DEFINITIONS} )
include_directories ( ${EIGEN3_INCLUDE_DIRS} )

include_directories(${Boost_INCLUDE_DIRS} include)

include_directories(cpp-utils/include)
ADD_LIBRARY(cpp-utils STATIC cpp-utils/src/network.cpp cpp-utils/src/conversion.cpp cpp-utils/src/files.cpp cpp-utils/src/json.cpp cpp-utils/src/math.cpp cpp-utils/src/output.cpp cpp-utils/src/system.cpp)

ADD_LIBRARY(utils-lib STATIC src/serialization.cpp src/config_handler.cpp)

add_executable(panda-controller src/main.cpp src/motion_generator.cpp src/task_handler.cpp src/printer.cpp src/udp_server.cpp src/tcp_server.cpp src/utils.cpp src/skills.cpp)
target_link_libraries(panda-controller Franka::Franka Threads::Threads  ${Boost_LIBRARIES} utils-lib yaml-cpp stdc++fs cpp-utils)
