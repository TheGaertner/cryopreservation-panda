#include "skillhandler.h"

int counter = 0;

void SkillHandler::setActualMarker(QListWidgetItem *item)
{
    actual_marker_ = item->text().toInt();
    initial_postion = Eigen::Map<Eigen::Matrix<double, 4, 4> >(lastState_.state_->O_T_EE.data());

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
    std::string command = "Hi: " + std::to_string(counter++);


    Eigen::Matrix<double, 4, 4> O_T_EE = Eigen::Map<Eigen::Matrix<double, 4, 4> >(lastState_.state_->O_T_EE.data());

    Eigen::Matrix<double, 4, 4> rel_position = initial_postion.inverse()*O_T_EE;

    return command;
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

SkillHandler::SkillHandler(QObject *parent) : QObject(parent)
{

}
