#include "devicehandler.h"

using namespace std;

DEVICE DeviceHandler::get_device(int id)
{
    for (auto entry : devices){
        if(entry.id == id){
            return entry;
        }
    }

    DEVICE device;
    device.id = id;
    device.marker_size = 0.50;
    device.name = "Not saved Device";
    device.position = Eigen::Matrix<double, 4, 4>::Zero();
    device.seen_since_startup = false;

    return device;
}

void DeviceHandler::update_device(int id, DEVICE device)
{
    for (auto &entry : devices) {
        if(entry.id == id){
            entry = device;
        }
    }
    save_devices();
}

DeviceHandler::DeviceHandler()
{
    YAML::Node config = ConfigHandler::getConfig("devices.yaml");

    vector<string> device_ids = config["Devices"].as<vector<string>>();

    devices.clear();
    for (auto entry : device_ids) {
        DEVICE device;
        device.id = stoi(entry);
        device.name = config[entry]["Name"].as<string>();
        device.marker_size = config[entry]["MarkerSize"].as<double>();

        std::vector<double> position = config[entry]["Position"].as<std::vector<double>>();
        device.position = Eigen::Matrix<double, 4, 4>(position.data());

        device.seen_since_startup = config[entry]["SeenSinceStartup"].as<bool>();

        devices.push_back(device);
    }
}

void DeviceHandler::save_devices()
{
    YAML::Node config;

    vector<string> device_ids;

    config["Devices"] = device_ids;

    for (auto entry : devices) {
        device_ids.push_back(to_string(entry.id));

        config[to_string(entry.id)]["Name"]=entry.name;
        config[to_string(entry.id)]["MarkerSize"]=entry.marker_size;

        config[to_string(entry.id)]["Position"]= std::vector<double>(entry.position.data(), entry.position.data() + entry.position.rows() * entry.position.cols());
        config[to_string(entry.id)]["SeenSinceStartup"]=entry.seen_since_startup;

    }
    config["Devices"] = device_ids;

    ConfigHandler::updateConfig(config,"devices.yaml");
}

void DeviceHandler::setAllToUnseen(){
    YAML::Node config = ConfigHandler::getConfig("devices.yaml");

    vector<string> device_ids = config["Devices"].as<vector<string>>();

    devices.clear();
    for (auto entry : device_ids) {
        DEVICE device;
        device.id = stoi(entry);
        device.name = config[entry]["Name"].as<string>();
        device.marker_size = config[entry]["MarkerSize"].as<double>();

        std::vector<double> position = config[entry]["Position"].as<std::vector<double>>();
        device.position = Eigen::Matrix<double, 4, 4>(position.data());

        device.seen_since_startup = false;

        devices.push_back(device);
    }

    save_devices();
}
