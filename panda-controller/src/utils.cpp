#include "utils.h"

bool test_robot_connection(std::string ip){
    try{
        franka::Robot* robot =  new franka::Robot(ip);
        delete robot;
        return true;
    }catch(const franka::NetworkException& e){
        cpp_utils::print_info("Skipping device with ip "+ip+".");
        return false;
    }catch(const franka::IncompatibleVersionException& e){
        cpp_utils::print_error("At device with ip "+ip+": ");
        std::cout<<e.what()<<std::endl;
        cpp_utils::print_warning("This robot possibly needs an update.");
        return false;
    }

    return false;
}

std::string find_robot(){
    std::string robot_address="none";
    std::string robot_iface="none";

    std::map<std::string,std::string> ifaces = cpp_utils::get_subnets();
    for(std::pair<std::string,std::string> i : ifaces){
        if(i.first=="lo" || i.first=="docker0" || i.first=="tap0"){
            continue;
        }
        for(unsigned j=1;j<255;j++){
            std::string address=i.second+std::to_string(j);
            if(cpp_utils::ping(address.c_str())==1){
                continue;
            }else{
                if(test_robot_connection(address)){
                    robot_address=address;
                    break;
                }
            }
        }
        if(robot_address!="none"){
            robot_iface=i.first;
            break;
        }
    }

    if(robot_address=="none"){
        cpp_utils::print_error("No connected robot found. Make sure that master controller and this computer share the same network and that the connection is properly configured.");
    }else{
        cpp_utils::print_success("Found robot at ip "+robot_address+" at interface "+robot_iface+".");
    }
    return robot_address;
}

std::string idToRobotMode(int id){
    std::string mode;

    switch(id){
    case 0:
        mode="Other";
        break;
    case 1:
        mode="Idle";
        break;
    case 2:
        mode="Move";
        break;
    case 3:
        mode="Guiding";
        break;
    case 4:
        mode="Reflex";
        break;
    case 5:
        mode="UserStopped";
        break;
    case 6:
        mode="AutomaticErrorRecovery";
        break;
    default:
        mode="No mode?";
    }

    return mode;
}


