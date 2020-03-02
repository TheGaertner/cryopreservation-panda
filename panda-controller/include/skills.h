#ifndef SKILLS_H
#define SKILLS_H

#include <boost/range/numeric.hpp>

#include <Eigen/Dense>

#include <franka/robot.h>
#include <franka/gripper.h>

#include "motion_generator.h"
#include "printer.h"

class Skills
{
public:
    void automaticErrorRecovery();
    void goToInitialPositon();
    void touchR();
    void relPose(double x, double y, double z, double xx, double yy, double zz, double duration=15);
    void absPose(Eigen::Matrix<double, 4, 4> goal_pose, double duration=15);


    Skills(franka::Robot *robot,franka::Gripper *gripper, Printer *printer);
private:
    franka::Robot *robot_;
    franka::Gripper *gripper_;
    Printer *printer_;
};

#endif // SKILLS_H
