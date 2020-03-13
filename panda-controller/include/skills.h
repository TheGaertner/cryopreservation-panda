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
    void push(Eigen::Vector3d direction, double distance);
    void setCollisionValue(int value);
    void setContactValue(int value);
    void updateCollisionContactValue();
    void setDefaultBehavior();
    void automaticErrorRecovery();
    void goToInitialPositon();
    void touchR();
    void relPose(double x, double y, double z, double xx, double yy, double zz, double duration=15,bool EE_frame = false);
    void joint_pose(double q0,double q1, double q2,double q3,double q4,double q5,double q6, double speedfactor);
    void absPose(Eigen::Matrix<double, 4, 4> goal_pose, double duration=15);


    Skills(franka::Robot *robot,franka::Gripper *gripper, Printer *printer);
private:
    franka::Robot *robot_;
    franka::Gripper *gripper_;
    Printer *printer_;

    int joint_contact_;
    int joint_collision_;
    int cartesian_contact_;
    int cartesian_collision_;
};

#endif // SKILLS_H
