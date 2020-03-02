#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include <fstream>
#include <experimental/filesystem>

#include <yaml-cpp/yaml.h>

extern std::experimental::filesystem::path config_location;


class ConfigHandler
{
public:
    static void setConfig(std::experimental::filesystem::__cxx11::path sd, bool global);
    static YAML::Node getConfig();
    static void updateConfig(YAML::Node config);

    ConfigHandler();
};

#endif // CONFIGHANDLER_H
