#include "skillhandler.h"

void SkillHandler::setActualMarker(QListWidgetItem *item)
{
    actual_marker_id_ = item->text().toInt();
    std::tuple<std::vector<int>,std::vector<Eigen::Matrix<double, 4, 4>>> output;
    Marker marker_detector(&lastState_);
    output = marker_detector.get_marker(videostream_->last_image_);


    for(unsigned long i = 0; i < std::get<0>(output).size(); i++){
        if(std::get<0>(output)[i] == actual_marker_id_)
        {
            marker_position = std::get<1>(output)[i];
        }
    }

    DeviceHandler device_handler;
    DEVICE device = device_handler.get_device(actual_marker_id_);
    device.position = marker_position;
    device_handler.update_device(actual_marker_id_,device);
}

void SkillHandler::setLastState(StateSerialization *state)
{
    lastState_ = *state;
}

void SkillHandler::setActualPose()
{
    goal_pose_ = Eigen::Map<Eigen::Matrix<double, 4, 4> >(lastState_.state_->O_T_EE.data());
    joint_pose_ = Eigen::Map<Eigen::Matrix<double, 7, 1> >(lastState_.state_->q.data());
}

void SkillHandler::setDuration(double time)
{
    qDebug() << time;
    duration = time;
}

std::string SkillHandler::create_relative_pose()
{
    Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(lastState_.state_->O_T_EE.data());

    Eigen::Matrix<double, 4, 4> rel_position = marker_position.inverse()*O_T_EE;

    std::string command = "RelToMarker ";
    command += std::to_string(actual_marker_id_);
    command += " ";
    for(int i = 0; i < rel_position.size(); i ++){
         command += std::to_string(rel_position(i));
         command += " ";
    }

    return command;
}

std::string SkillHandler::go_to_relative_pose(std::string command)
{
    std::vector<std::string> strs;
    boost::split(strs, command, boost::is_any_of(" "));

    int marker_id = std::stoi(strs[1]);

    DeviceHandler device_handler;
    DEVICE device = device_handler.get_device(marker_id);

    Eigen::Matrix<double, 4, 4> relative_postion;
    for(int i = 0; i < 16; i++){
        relative_postion(i)=std::stod(strs[2+i]);
    }

    Eigen::Matrix<double, 4, 4> abs_position = device.position*relative_postion;

    std::string command_new = "AbsPose";

    for(int j = 0; j < 16; j++){
        command_new+= " ";
        command_new+= std::to_string(abs_position(j%4,j/4)+0.0000000001);
    }
    std::cout << command_new << std::endl;

    return command_new;

}

std::string SkillHandler::go_to_absolute_pose(Eigen::Matrix<double, 4, 4> pose)
{

}

std::string SkillHandler::create_joint_position()
{
    std::string command = "JointPose";
    for(int i = 0 ; i < joint_pose_.size(); i++){
        command += " " + std::to_string(joint_pose_[i]);
    }
    command += " " + std::to_string(duration);

    return command;

}

SkillHandler::SkillHandler(Videostream* videostream, QObject *parent)
{
    videostream_ = videostream;
    DeviceHandler();
}
