#include "config_handler.h"

std::experimental::filesystem::path config_location;

void ConfigHandler::setConfig(std::experimental::filesystem::path command, bool global = false){

    char buff[512];
    FILE *fpipe = popen("pwd","r");
    std::fgets(buff,sizeof(buff)-2,fpipe);

    std::string pwd = buff;
    pwd.pop_back();

    std::experimental::filesystem::path pwd_path;
    if(command.is_relative()){
        pwd+=command.string().substr(1,command.string().length()-1);

        pwd_path = pwd;
        pwd_path = pwd_path.remove_filename().parent_path();

        config_location = pwd_path;
    }else{
        config_location = command.remove_filename().parent_path();

    }

    if(global){
        config_location = config_location.parent_path();
    }
    config_location/="config.yaml";
}

YAML::Node ConfigHandler::getConfig(){

    YAML::Node config;
    if(std::experimental::filesystem::exists(config_location)){
        config = YAML::LoadFile(config_location);
    }else{
        config_location = config_location.remove_filename();
        config_location/="default_config.yaml";
        config = YAML::LoadFile(config_location);
    }

    return config;
}


void ConfigHandler::updateConfig(YAML::Node config){
    std::ofstream fout(config_location);
    fout << config;
    fout.flush();
}



ConfigHandler::ConfigHandler()
{

}
