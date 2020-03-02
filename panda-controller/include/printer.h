#ifndef PRINTER_H
#define PRINTER_H

#include <mutex>
#include <thread>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <franka/robot_state.h>

#include "udp_server.h"
#include "serialization.h"


// Initialize data fields for the print thread.
struct Print_data{
    std::mutex mutex;
    bool has_data;
    franka::RobotState robot_state;
    std::array<double, 7> gravity;
};


class Printer
{
public:
    void update_state(franka::RobotState state);

    Printer();
    ~Printer();

    StateSerialization serialized_state_;

private:
    void send_loop();

    int print_rate = 10;
    bool running_ = true;
    std::thread mythread;
    Print_data print_data;
    boost::asio::io_service io_service_;
    udp_server udp_server_;
};

#endif // PRINTER_H
