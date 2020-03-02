#include <franka/robot.h>
#include <franka/gripper.h>

#include "task_handler.h"
#include "tcp_server.h"
#include "config_handler.h"

#include "cpp_utils/network.hpp"

int main(int argc, char *argv[])
{
    // Load config
    ConfigHandler::setConfig(argv[0],false);
    YAML::Node config = ConfigHandler::getConfig();

    // Stream webcam data over udp
    std::thread camera_thread_0([=](){
        if(config["camera_0_on"].as<bool>()){

            auto debug = system("chmod +x ../start_camera.sh ");
            std::string command = "../start_camera.sh '"+ config["camera_0_name"].as<std::string>() +"' "+config["client_hostname"].as<std::string>()+" "+ config["camera_0_port"].as<std::string>() +" >/dev/null";
            debug = system(command.c_str());
            std::cout << "Camera 0 started!" << std::endl;
        }
    });

    std::thread camera_thread_1([=](){
        if(config["camera_1_on"].as<bool>()){
            auto debug = system("chmod +x ../start_camera.sh ");
            std::string command = "../start_camera.sh '"+ config["camera_1_name"].as<std::string>() +"' "+config["client_hostname"].as<std::string>()+" "+ config["camera_1_port"].as<std::string>() +" >/dev/null";
            debug = system(command.c_str());
            std::cout << "Camera 1 started!" << std::endl;
        }

    });

    // Open roboter connection
    if(config["robot_ip"].as<std::string>().size()<2){
        std::cout << "Robot IP is not set... searching for robot!" << std::endl;
        std::string ip = find_robot();
        config["robot_ip"] = ip;
        ConfigHandler::updateConfig(config);
    }else{
        std::cout << "Robot IP was set to: " << config["robot_ip"].as<std::string>() << std::endl;
    }

    franka::Robot robot(config["robot_ip"].as<std::string>());
    franka::Gripper gripper(config["robot_ip"].as<std::string>());
    std::cout << "Panda connected!" << std::endl;

    // Create task handler
    task_handler task_handler(&robot, &gripper);


    // Open TCP connection for incoming commands
    boost::asio::io_service io_service;
    tcp_server tcp_server(io_service,config["nuc_tcp_port"].as<int>(),&task_handler);
    std::thread th([&] { io_service.run(); });


    // Wait for incoming commands
    while(true){
        std::this_thread::sleep_for (std::chrono::milliseconds(static_cast<int>(1)));

        task_handler.execute_task();
    }

    // Cleanup
    camera_thread_0.detach();
    camera_thread_0.~thread();
    camera_thread_1.detach();
    camera_thread_1.~thread();
    th.detach();
    th.~thread();

    return 0;
}
