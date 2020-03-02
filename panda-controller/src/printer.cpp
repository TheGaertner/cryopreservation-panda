#include "printer.h"

void Printer::send_loop(){

    while(running_){
        std::this_thread::sleep_for(
                    std::chrono::milliseconds(static_cast<int>(10)));

        // Try to lock data to avoid read write collisions.
        if (print_data.mutex.try_lock()) {
            if (print_data.has_data) {
                serialized_state_.update_state(&print_data.robot_state);
                serialized_state_.update_time();
                serialized_state_.robot_mode_converted = int(print_data.robot_state.robot_mode);

                // serialize state
                boost::asio::streambuf buffer;
                {
                    std::ostream os(&buffer);
                    boost::archive::binary_oarchive out_archive(os);
                    out_archive << serialized_state_;
                }

                // Send serialized state
                udp_server_.start_send(&buffer);


                // std::cout << "Debug: " << print_data.robot_state.q[0] << std::endl;

                print_data.has_data = false;

            }
            print_data.mutex.unlock();

        }
    }
}

Printer::Printer():udp_server_(io_service_), mythread{}
{

    mythread = std::thread(&Printer::send_loop, this);
}


Printer::~Printer()
{
    mythread.detach();
    mythread.~thread();
}

void Printer::update_state(franka::RobotState state)
{
    if (print_data.mutex.try_lock()) {
        print_data.has_data = true;
        print_data.robot_state = state;
        print_data.mutex.unlock();

    }
}
