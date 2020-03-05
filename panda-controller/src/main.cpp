#include <franka/robot.h>
#include <franka/gripper.h>

#include "task_handler.h"
#include "tcp_server.h"
#include "config_handler.h"

#include "cpp_utils/network.hpp"

extern franka::Robot robot;
extern franka::Gripper gripper;

int main(int argc, char *argv[])
{
    // Load config
    ConfigHandler::setConfig(argv[0],false);
    YAML::Node config = ConfigHandler::getConfig();

    // Stream webcam data over udp
    std::vector<std::string> broadcast_adresse_parts = cpp_utils::split_string(cpp_utils::get_ip_by_hostname(config["nuc"]["hostname"].as<std::string>()),".");
    std::string broadcast_adresse_merged = broadcast_adresse_parts[0]+"."+broadcast_adresse_parts[1]+"."+broadcast_adresse_parts[2]+".255";


    std::thread camera_thread_0([=](){
        if(config["nuc"]["camera_0"]["on"].as<bool>()){

            auto debug = system("chmod +x ../start_camera.sh ");
            std::string command = "../start_camera.sh '"+ config["nuc"]["camera_0"]["name"].as<std::string>() +"' "+broadcast_adresse_merged+" "+ config["nuc"]["camera_0"]["port"].as<std::string>() +" >/dev/null";
            debug = system(command.c_str());
            std::cout << "Camera 0 started!" << std::endl;
        }
    });

    std::thread camera_thread_1([=](){
        if(config["nuc"]["camera_1"]["on"].as<bool>()){
            auto debug = system("chmod +x ../start_camera.sh ");
            std::string command = "../start_camera.sh '"+ config["nuc"]["camera_1"]["name"].as<std::string>() +"' "+broadcast_adresse_merged+" "+ config["nuc"]["camera_1"]["port"].as<std::string>() +" >/dev/null";
            debug = system(command.c_str());
            std::cout << "Camera 1 started!" << std::endl;
        }

    });

    // Open roboter connection
    if(config["robot"]["ip"].as<std::string>().size()<2){
        std::cout << "Robot IP is not set... searching for robot!" << std::endl;
        std::string ip = find_robot();
        config["robot"]["ip"] = ip;
        ConfigHandler::updateConfig(config);
    }else{
        std::cout << "Robot IP was set to: " << config["robot"]["ip"].as<std::string>() << std::endl;
    }


    if(cpp_utils::ping(config["robot"]["ip"].as<std::string>().c_str())){
        cpp_utils::print_error("Can't ping robot. Robot IP wrong or robot is shut down.");
        cpp_utils::print_error("Software will shut down.");
        return 1;
    }

    franka::Robot robot(config["robot"]["ip"].as<std::string>());
    franka::Gripper gripper(config["robot"]["ip"].as<std::string>());
    std::cout << "Panda connected!" << std::endl;

    // Create task handler
    task_handler task_handler(&robot, &gripper);


    // Open TCP connection for incoming commands
    boost::asio::io_service io_service;
    tcp_server tcp_server(io_service,config["nuc"]["tcp_port"].as<int>(),&task_handler);
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
