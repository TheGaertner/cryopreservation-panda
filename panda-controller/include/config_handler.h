#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include <yaml-cpp/yaml.h>

extern std::experimental::filesystem::path config_location;


class ConfigHandler
{
public:
    static void setConfig(std::experimental::filesystem::__cxx11::path sd, bool global = false, std::string name = "main.yaml");
    static YAML::Node getConfig(std::string name = "main.yaml");
    static void updateConfig(YAML::Node config, std::string name = "main.yaml");

    ConfigHandler();
};

#endif // CONFIGHANDLER_H
