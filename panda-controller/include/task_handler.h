#ifndef TASK_HANDLER_H
#define TASK_HANDLER_H

#include <queue>
#include <functional>

#include <boost/algorithm/string.hpp>

#include <franka/robot.h>
#include <franka/gripper.h>

#include "cpp_utils/output.hpp"

#include "printer.h"
#include "utils.h"
#include "skills.h"

class task_handler
{
public:
    void add_task(std::string name);
    void clearTaskList();
    void execute_task();
    task_handler(franka::Robot *robot, franka::Gripper *gripper);
private:
    std::queue<std::string> task_list_;
    franka::Robot* robot_;
    franka::Gripper* gripper_;
    Printer* printer_;
    Print_data print_data_;
    int robot_mode = -1;
    Skills *skills_;

};

#endif // TASK_HANDLER_H
