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

include_directories(${Boost_INCLUDE_DIRS} include cpp-utils/include)

ADD_LIBRARY(cpp-utils STATIC cpp-utils/src/network.cpp cpp-utils/include/cpp_utils/network.hpp cpp-utils/src/conversion.cpp cpp-utils/include/cpp_utils/conversion.hpp cpp-utils/include/cpp_utils/network.hpp  cpp-utils/src/files.cpp cpp-utils/include/cpp_utils/files.hpp cpp-utils/src/json.cpp cpp-utils/include/cpp_utils/json.hpp cpp-utils/src/math.cpp cpp-utils/include/cpp_utils/math.hpp cpp-utils/src/output.cpp cpp-utils/include/cpp_utils/output.hpp cpp-utils/src/system.cpp cpp-utils/include/cpp_utils/system.hpp)

ADD_LIBRARY(utils-lib STATIC src/serialization.cpp include/serialization.h src/config_handler.cpp include/config_handler.h)

add_executable(panda-controller src/main.cpp src/motion_generator.cpp include/motion_generator.h src/task_handler.cpp include/task_handler.h src/printer.cpp include/printer.h src/udp_server.cpp include/udp_server.h src/tcp_server.cpp include/tcp_server.h src/utils.cpp include/utils.h src/skills.cpp include/skills.h)
target_link_libraries(panda-controller Franka::Franka Threads::Threads  ${Boost_LIBRARIES} utils-lib yaml-cpp stdc++fs cpp-utils)
