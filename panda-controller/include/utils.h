#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <array>
#include <iterator>

#include <franka/robot.h>
#include <franka/exception.h>

#include "cpp_utils/network.hpp"
#include "cpp_utils/output.hpp"

template <class T, size_t N>
std::ostream& operator<<(std::ostream& ostream, const std::array<T, N>& array) {
    ostream << "[";
    std::copy(array.cbegin(), array.cend() - 1, std::ostream_iterator<T>(ostream, ","));
    std::copy(array.cend() - 1, array.cend(), std::ostream_iterator<T>(ostream));
    ostream << "]";
    return ostream;
}


bool test_robot_connection(std::string ip);
std::string find_robot();
std::string idToRobotMode(int id);

#endif // UTILS_H
