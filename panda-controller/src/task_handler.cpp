#include "task_handler.h"

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
        task_list_.push(name);
//        std::cout << "Added: " +name+ " to task_handler. Elements in list now: " << task_list_.size() << std::endl;
    }

}

void task_handler::execute_task()
{
    try
    {

        if(!task_list_.empty() && (robot_->readOnce().robot_mode != franka::RobotMode::kUserStopped)){
            skills_->setDefaultBehavior();

            std::string name = task_list_.front();
            task_list_.pop();


            if(name == "AutoErrorRecovery" ){
                std::cout << "Execute recovery!" << std::endl;
                skills_->automaticErrorRecovery();
            }

            if(name == "GoToInit"){
                std::cout << "___________________________" << std::endl;
                std::cout << "Go to initial position!" << std::endl;
                skills_->goToInitialPositon();
                std::cout << "Go to initial position finished!" << std::endl;

            }

            if(name == "TouchR"){
                std::cout << "___________________________" << std::endl;
                std::cout << "Swing around z-axis" << std::endl;

            }

            if(name.find("Gripper")==0){ // Gripper 0.05 0.1, 60
                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " << name << std::endl;
                std::vector<std::string> strs;
                boost::split(strs, name, boost::is_any_of(" "));

                double grasping_width;
                double speed;
                double force;
                if(strs.size()>1){
                    grasping_width = std::stod(strs[1]);
                    speed = std::stod(strs[2]);
                    force = std::stod(strs[3]);
                }

                switch (strs.size()) {
                case 1:
                {
                    gripper_->homing();
                    franka::GripperState gripper_state = gripper_->readOnce();
                    std::cout << "Gripper width: " << gripper_state.max_width << std::endl;
                    break;
                }
                case 5:
                {
                    gripper_->homing();
                    franka::GripperState gripper_state = gripper_->readOnce();
                    if (gripper_state.max_width < grasping_width) {
                        std::cout << "Object is too large for the current fingers on the gripper." << std::endl;
                        break;
                    };
                }
                default:
                    if (!gripper_->grasp(grasping_width, speed, force,0.01,0.01)) {
                        std::cout << "Failed to grasp object." << std::endl;
                    }

                }
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


            if(name.find("RelPose")==0){
                std::cout << "___________________________" << std::endl;
                std::cout << "Start: " << name << std::endl;
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

                if(strs.size()>4){
                    target_xx = std::stod(strs[4]);
                    target_yy = std::stod(strs[5]);
                    target_zz = std::stod(strs[6]);

                }

                skills_->relPose(target_x,target_y,target_z,target_xx,target_yy,target_zz,duration);

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
}
