#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H

#include <Eigen/Dense>

#include "config_handler.h"

struct DEVICE {
    int id;
    double marker_size;
    std::string name;
    Eigen::Matrix<double, 4, 4> position;
    bool seen_since_startup;
};

class DeviceHandler
{
public:
    DEVICE get_device(int id);
    void update_device(int id, DEVICE device);
    Eigen::Matrix<double, 4, 4> getLastPosition(int id);
    DeviceHandler();
    void setAllToUnseen();
private:
    void save_devices();
    std::vector<DEVICE> devices;
};

#endif // DEVICEHANDLER_H
