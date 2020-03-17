#include "task_handler.h"

using namespace std;

void task_handler::clearTaskList()
{
    std::queue<std::string> empty;
    std::swap( task_list_, empty );
}

void task_handler::add_task(std::string name)
{
    if(name=="StopAll"){
        robot_->stop();
        gripper_->stop();
        clearTaskList();

        std::cout << "Stopped current task and cleared task list" << std::endl;
    }else{
        boost::replace_all(name, ",", ".");
        task_list_.push(name);
        std::cout << "Added: " +name+ " to task_handler. Elements in list now: " << task_list_.size() << std::endl;
    }

}

void task_handler::execute_task()
{
    try
    {
        if(!task_list_.empty() && (robot_->readOnce().robot_mode == franka::RobotMode::kIdle)){


            std::string name = task_list_.front();
            task_list_.pop();

            if(name == "SetDefaulBehavior" ){
                std::cout << "SetDefaulBehavior!" << std::endl;
                skills_->setDefaultBehavior();
            }

            if(name == "AutoErrorRecovery" ){
                std::cout << "Execute recovery!" << std::endl;
                skills_->automaticErrorRecovery();
            }

            if(name.find("SetColConBehaviour")== 0){
                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " + name << std::endl;

                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));

                skills_->setCollisionValue( std::stod(strs[1]));
                skills_->setContactValue( std::stod(strs[2]));

                std::cout << "Contact and Collision Force updated to:  Collison: " +std::to_string(std::stod(strs[1]))+"N and Contact: " +std::to_string(std::stod(strs[2]))+ "N"  << std::endl;
            }

            if(name == "GoToInit"){
                std::cout << "___________________________" << std::endl;
                std::cout << "Go to initial position!" << std::endl;
                skills_->goToInitialPositon();
                std::cout << "Go to initial position finished!" << std::endl;
            }


            if(name.find("CartesianImpedance")== 0){
                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " << name << std::endl;
                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));
                double target_x = std::stod(strs[1]);
                double target_y = std::stod(strs[2]);
                double target_z = std::stod(strs[3]);

                double target_xx = std::stod(strs[4]);
                double target_yy = std::stod(strs[5]);
                double target_zz = std::stod(strs[6]);

                skills_->setCartesianImpedance(target_x,target_y,target_z,target_xx,target_yy,target_zz);

                std::cout << "Cartesian Impedance set!"<< std::endl;
            }

            if(name.find("JointPose")== 0){
                std::cout << "___________________________" << std::endl;
                std::cout << name << std::endl;
                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));

                skills_->joint_pose(std::stod(strs[1]),std::stod(strs[2]),std::stod(strs[3]),std::stod(strs[4]),std::stod(strs[5]),std::stod(strs[6]),std::stod(strs[7]),std::stod(strs[8]));
                std::cout << "Go to joint position finished!" << std::endl;
            }

            if(name.find("Gripper")==0){ // Gripper 0.05 0.1, 60
                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " << name << std::endl;
                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));



                if(name.find("Gripper Homing")==0)  {
                    gripper_->homing();
                    franka::GripperState gripper_state = gripper_->readOnce();
                    std::cout << "Gripper width: " << gripper_state.max_width << std::endl;
                }else{
                    double grasping_width;
                    double speed;
                    double force;
                    double tolerance;
//                    if(strs.size()>1){
                        grasping_width = std::stod(strs[1]);
                        speed = std::stod(strs[2]);
                        force = std::stod(strs[3]);
                        tolerance = std::stod(strs[4]);
//                    }


                    if (!gripper_->grasp(grasping_width, speed, force,tolerance,tolerance)) {
                        std::cout << "Failed to grasp object." << std::endl;
                    }else{
                        std::cout << "Object grasped!" << std::endl;
                    }

                }
                std::cout << "Gripping finished!" << std::endl;
            }

            if(name.find("AbsPose")==0){
                // Create Inputs
                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));



                Eigen::Matrix<double, 4, 4>goal_pose = Eigen::Array44d::Zero();

                for(int i = 0; i < 16; i++){
                    goal_pose(i%4,i/4) = std::stod(strs[i+1]);
                }

                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " << strs[0] << std::endl;
                std::cout << goal_pose << std::endl;

                skills_->absPose(goal_pose);

                std::cout << "AbsPose finished!"<< std::endl;
            }

            if(name.find("RelTurn")== 0){
                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " << name << std::endl;
                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));
                double time = 0;
                double duration = 15;
                double target_x = std::stod(strs[1]);
                double target_y = std::stod(strs[2]);
                double target_z = std::stod(strs[3]);

                skills_->relPose(0,0,0,target_x,target_y,target_z,duration);

                std::cout << "RelTurn finished!"<< std::endl;
            }


            if(name.find("RelPose")==0 ){
                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " << name << std::endl;
                std::cout.flush();
                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));

                double time = 0;
                double duration = 15;
                double target_x = std::stod(strs[1]);
                double target_y = std::stod(strs[2]);
                double target_z = std::stod(strs[3]);

                double target_xx = 0;
                double target_yy = 0;
                double target_zz = 0;

                bool EE_frame = false;
                if (strs.size()==9){
                    EE_frame = true;
                }

                if(strs.size()>4){
                    target_xx = std::stod(strs[4]);
                    target_yy = std::stod(strs[5]);
                    target_zz = std::stod(strs[6]);

                }

                skills_->relPose(target_x,target_y,target_z,target_xx,target_yy,target_zz,duration,EE_frame);

                std::cout << "RelPose finished!"<< std::endl;
            }

            if(name.find("Push")==0){
                // Create Inputs
                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));

                Eigen::Vector3d direction(std::stod(strs[1]),std::stod(strs[2]),std::stod(strs[3]));
                double distance = std::stod(strs[4]);

                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " << name << std::endl;

                skills_->push(direction,distance);

                std::cout << strs[0]+" finished!"<< std::endl;
            }

        }

        franka::RobotState state = robot_->readOnce();


        if(robot_mode != (int)state.robot_mode){
            robot_mode = (int)state.robot_mode;


            std::cout << "Current robot mode: " << idToRobotMode(robot_mode) << std::endl;
        }


        printer_ ->update_state(robot_->readOnce());
    }
    catch (std::exception& e)
    {
        cpp_utils::print_warning(e.what());
        std::cerr << "Task cancled" << std::endl;

        std::cerr << "Execute Autorecovery" << std::endl;
        robot_->automaticErrorRecovery();



    }
}

task_handler::task_handler(franka::Robot *robot, franka::Gripper *gripper):robot_(robot),gripper_(gripper)
{
    printer_ = new Printer();
    skills_ = new Skills(robot,gripper,printer_);
    task_list_.push("SetDefaulBehavior");
}
