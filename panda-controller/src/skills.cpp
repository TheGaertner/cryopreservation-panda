#include "skills.h"

void Skills::push(Eigen::Vector3d direction, double distance)
{
    Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(robot_->readOnce().O_T_EE.data());
    Eigen::Matrix<double, 3, 3> O_T_EE_rotation = O_T_EE.block(0,0,3,3);

    direction = O_T_EE_rotation.inverse()*direction;
    direction.normalize();
    direction = direction*distance;

    setCollisionValue(100);
    relPose(direction[0],direction[1],direction[2],0,0,0);
    relPose(-direction[0],-direction[1],-direction[2],0,0,0);
    setDefaultBehavior();
}

void Skills::setDefaultBehavior() {
    joint_collision_ = 20;
    joint_contact_ = 7;
    cartesian_collision_ = 20;
    cartesian_contact_ = 7;

    updateCollisionContactValue();

    robot_->setJointImpedance({{3000, 3000, 3000, 2500, 2500, 2000, 2000}});
    robot_->setCartesianImpedance({{3000, 3000, 300, 300, 300, 300}});
}


void Skills::updateCollisionContactValue()
{
    std::array<double, 7>joint_collision;
    joint_collision.fill(joint_collision_);
    std::array<double, 7>joint_contact;
    joint_contact.fill(joint_contact_);

    std::array<double, 6>cartesian_collision;
    cartesian_collision.fill(cartesian_collision_);
    std::array<double, 6>cartesian_contact;
    cartesian_contact.fill(cartesian_contact_);

    robot_->setCollisionBehavior(
                joint_contact, joint_collision,
                joint_contact, joint_collision,
                cartesian_contact, cartesian_collision,
                cartesian_contact, cartesian_collision);
}

void Skills::setCollisionValue(int value)
{
    joint_collision_ = value;
    cartesian_collision_ = value;

    updateCollisionContactValue();
}

void Skills::setContactValue(int value)
{
    joint_contact_ = value;
    cartesian_contact_ = value;

    updateCollisionContactValue();
}

void Skills::setCartesianImpedance(double x, double y, double z, double xx, double yy, double zz)
{
     robot_->setCartesianImpedance({{x, y, z, xx, yy, zz}});
}


void Skills::automaticErrorRecovery()
{
    robot_->automaticErrorRecovery();
}

void Skills::goToInitialPositon()
{
    setDefaultBehavior();
    std::array<double, 7> q_goal = {{0, -M_PI_4, 0, -3 * M_PI_4,0, M_PI_2, M_PI_4}};
    MotionGenerator motion_generator(0.5, q_goal);
    robot_->control(motion_generator);
}

void Skills::touchR()
{
    double time_max = 4.0;
    double omega_max = 0.2;
    double time = 0.0;

    robot_->control([=, &time](const franka::RobotState& state,
                    franka::Duration period) -> franka::JointVelocities {
        time += period.toSec();
        double cycle = std::floor(std::pow(-1.0, (time - std::fmod(time, time_max)) / time_max));
        double omega = cycle * omega_max / 2.0 * (1.0 - std::cos(2.0 * M_PI / time_max * time));
        franka::JointVelocities velocities = {{-omega, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};

        if (time >= 2 * time_max || (boost::accumulate(state.cartesian_contact,0) > 0)) {
            std::cout << std::endl << "Finished motion." << std::endl;
            return franka::MotionFinished(velocities);
        }
        printer_ ->update_state(state);

        return velocities;
    });
}

void Skills::relPose(double x, double y, double z, double xx, double yy, double zz,double duration,bool EE_frame)
{
    if(EE_frame){
        Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(robot_->readOnce().O_T_EE.data());
        Eigen::Matrix<double, 3, 3> rotation = O_T_EE.block(0,0,3,3).inverse();

        Eigen::Matrix<double, 3, 1> input(x,y,z);
        Eigen::Matrix<double, 3, 1> output = rotation *input;
        x =  output.x();
        y =  output.y();
        z =  output.z();

        xx = 0;
        yy = 0;
        zz = 0;
    }

    double time = 0;
    auto cartesian_motion = [=,&x, &y, &z,&xx,&yy,&zz, &time, &duration](const franka::RobotState& robot_state,
            franka::Duration period) -> franka::CartesianVelocities {
        time += period.toSec();



        if(std::accumulate(robot_state.cartesian_contact.begin(), robot_state.cartesian_contact.end(), 0)){
            x *=0.7;
            y *=0.7;
            z *=0.7;
            xx *=0.7;
            yy *=0.7;
            zz *=0.7;
        }

        double v_x = 2*x*2*M_PI / duration * std::cos(2*M_PI / duration *time) * std::sin(2*M_PI / duration *time);
        double v_y = 2*y*2*M_PI / duration * std::cos(2*M_PI / duration *time) * std::sin(2*M_PI / duration *time);
        double v_z = 2*z*2*M_PI / duration * std::cos(2*M_PI / duration *time) * std::sin(2*M_PI / duration *time);
        double v_xx = 2*xx*2*M_PI / duration * std::cos(2*M_PI / duration *time) * std::sin(2*M_PI / duration *time);
        double v_yy = 2*yy*2*M_PI / duration * std::cos(2*M_PI / duration *time) * std::sin(2*M_PI / duration *time);
        double v_zz = 2*zz*2*M_PI / duration * std::cos(2*M_PI / duration *time) * std::sin(2*M_PI / duration *time);

        franka::CartesianVelocities new_pose = {{v_x, v_y, v_z, v_xx, v_yy, v_zz}};
        printer_ ->update_state(robot_state);

        if (time >= duration/4) {
            std::cout << std::endl << "Finished motion!" << std::endl;
            return franka::MotionFinished(new_pose);
        }
        return new_pose;
    };

    robot_->control(cartesian_motion,franka::ControllerMode::kCartesianImpedance,true,10);
}

void Skills::joint_pose(double q0, double q1, double q2, double q3, double q4, double q5, double q6, double speedfactor)
{
    setDefaultBehavior();
    std::array<double, 7> q_goal = {{q0,q1,q2,q3,q4,q5,q6,}};
    MotionGenerator motion_generator(speedfactor, q_goal);
    robot_->control(motion_generator);
}

void Skills::absPose(Eigen::Matrix<double, 4, 4> goal_pose, double duration)
{

    Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(robot_->readOnce().O_T_EE.data());

    Eigen::Matrix<double, 3, 3> goal_rotation = goal_pose.block(0,0,3,3); //*Eigen::Quaterniond(0,1,0,0)*Eigen::Quaterniond(0.7071068,0,0,-0.7071068);
    Eigen::Matrix<double, 3, 3> ee_rotation = O_T_EE.block(0,0,3,3);

    Eigen::Matrix<double, 3, 3> diff_rotation = ee_rotation.inverse()*goal_rotation; // O_T_EE.inv * O_T_M

    Eigen::AngleAxisd angle_axis;
    angle_axis.fromRotationMatrix(diff_rotation);

    Eigen::Vector3d axis = angle_axis.axis();
    axis = ee_rotation.inverse()*axis;

    relPose((goal_pose-O_T_EE)(0,3),
            (goal_pose-O_T_EE)(1,3),
            (goal_pose-O_T_EE)(2,3),
            axis(0)*angle_axis.angle(), // -
            axis(1)*angle_axis.angle(),
            axis(2)*angle_axis.angle());



}



Skills::Skills(franka::Robot *robot,franka::Gripper *gripper, Printer *printer):robot_(robot),gripper_(gripper),printer_(printer)
{

}
