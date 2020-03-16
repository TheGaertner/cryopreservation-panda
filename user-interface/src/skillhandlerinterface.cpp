#include "skillhandlerinterface.h"

#include <bitset>

void SkillHandlerInterface::addGroup()
{
    if (std::find(groups_.begin(), groups_.end(),new_group_name_)==groups_.end()){
        groups_.push_back(new_group_name_);
        updateGroupWidget();
    }
}

void SkillHandlerInterface::moveSequenceElementUp()
{
    std::vector<std::string>::iterator it = std::find(sequence_elements_.begin(), sequence_elements_.end(), selected_sequence_element_name_);

    if(it != sequence_elements_.begin()){
        std::string temp = *(it-1);
        *(it-1) = *it;
        *it = temp;
        updateSequenceWidget();
        emit setSequenceElementRow(it-sequence_elements_.begin()-1);
    }else{
        emit setSequenceElementRow(it-sequence_elements_.begin());
    }

    updateConfig();


}

void SkillHandlerInterface::moveSequenceElementDown()
{
    std::vector<std::string>::iterator it = std::find(sequence_elements_.begin(), sequence_elements_.end(), selected_sequence_element_name_);

    if(it != (sequence_elements_.end()-1)){
        std::string temp = *(it+1);
        *(it+1) = *it;
        *it = temp;
        updateSequenceWidget();
        emit setSequenceElementRow(it-sequence_elements_.begin()+1);
    }else{
        emit setSequenceElementRow(it-sequence_elements_.begin());
    }

    updateConfig();


}

void SkillHandlerInterface::removeGroup()
{
    std::vector<std::string>::iterator it = std::find(groups_.begin(), groups_.end(), selected_group_name_);
    groups_.erase(std::remove(groups_.begin(), groups_.end(), selected_group_name_), groups_.end());

    updateGroupWidget();
    emit setGroupRow(it-groups_.begin()-1);

    YAML::Node groups_dict = ConfigHandler::getConfig("skills.yaml");

    std::vector<std::string> groups_from_dict = groups_dict["Groups"].as<std::vector<std::string>>();
    groups_from_dict.erase(std::remove(groups_from_dict.begin(), groups_from_dict.end(), selected_group_name_), groups_from_dict.end());
    groups_dict["Groups"] = groups_from_dict;

    groups_dict.remove(selected_group_name_);

    ConfigHandler::updateConfig(groups_dict,"skills.yaml");

    if(it!=skills_.begin()){
        selected_skill_name_ = *(it-1);
    }else{
        selected_skill_name_ = *(it);
    }


}

void SkillHandlerInterface::init()
{
    YAML::Node devices_dict = ConfigHandler::getConfig("skills.yaml");
    groups_ = devices_dict["Groups"].as<std::vector<std::string>>();


    updateSkillWidget();
    updateGroupWidget();

    QStringList movement_primitves;
    movement_primitves.append("Relative Pose");
    movement_primitves.append("Joint Position");
    movement_primitves.append("Relative Movement");
    movement_primitves.append("Absolut Position");
    movement_primitves.append("Gripper");
    movement_primitves.append("Set Collision / Contact Force");
    movement_primitves.append("Set Cartesian Impedance");

    emit addMovementPrimitiveToWidget(movement_primitves);
}

void SkillHandlerInterface::updateConfig()
{
    YAML::Node skills_dict = ConfigHandler::getConfig("skills.yaml");

    skills_dict[selected_group_name_][selected_skill_name_]=sequence_elements_;

    if(skills_dict[selected_group_name_]["Skills"].size()!=0){
        std::vector<std::string> skills_from_dict = skills_dict[selected_group_name_]["Skills"].as<std::vector<std::string>>();
        if (std::find(skills_from_dict.begin(), skills_from_dict.end(),selected_skill_name_)==skills_from_dict.end()){
            skills_dict[selected_group_name_]["Skills"].push_back(selected_skill_name_);
        }
    }else{
        skills_dict[selected_group_name_]["Skills"].push_back(selected_skill_name_);
    }


    std::vector<std::string> groups_from_dict = skills_dict["Groups"].as<std::vector<std::string>>();
    if (std::find(groups_from_dict.begin(), groups_from_dict.end(),selected_group_name_)==groups_from_dict.end()){
        skills_dict["Groups"].push_back(selected_group_name_);
    }


    ConfigHandler::updateConfig(skills_dict,"skills.yaml");
}

void SkillHandlerInterface::playSequence()
{
    for(int i = 0; i < sequence_elements_.size(); i++){
        if(sequence_elements_[i] == "Stop"){
            break;
        }
        if(sequence_elements_[i].find("RelToMarker") == 0){
            emit command(QString::fromStdString(skill_handler_->go_to_relative_pose(sequence_elements_[i])));
            continue;
        }
        emit command(QString::fromStdString(sequence_elements_[i]));
    }

}

void SkillHandlerInterface::createRelativePose()
{
    std::string command = skill_handler_->create_relative_pose();
    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();

}

void SkillHandlerInterface::createJointPosition()
{
    std::string command = skill_handler_->create_joint_position();
    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();
}

void SkillHandlerInterface::createRelMovement(double x, double y, double z, double xx, double yy, double zz, double duration, bool EE_frame)
{
    std::string command = "RelPose";
    command += " " + std::to_string(x);
    command += " " + std::to_string(y);
    command += " " + std::to_string(z);
    command += " " + std::to_string(xx);
    command += " " + std::to_string(yy);
    command += " " + std::to_string(zz);
    command += " " + std::to_string(duration);
    if(EE_frame){
        command += " EE";
    }

    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();
}

void SkillHandlerInterface::gripper_action(double width, double speed, double force, double tolerance)
{
    std::string command = "Gripper";
    command += " " + std::to_string(width);
    command += " " + std::to_string(speed);
    command += " " + std::to_string(force);
    command += " " + std::to_string(tolerance);

    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();
}

void SkillHandlerInterface::gripper_homing()
{
    std::string command = "Gripper Homing";
    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();
}

void SkillHandlerInterface::set_colcon_behaviour(double col, double con)
{
    std::string command = "SetColConBehaviour";
    command += " " + std::to_string(col);
    command += " " + std::to_string(con);

    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();

}

void SkillHandlerInterface::set_cartesian_impedance(double x, double y, double z, double xx, double yy, double zz)
{
    std::string command = "CartesianImpedance";
    command += " " + std::to_string(x);
    command += " " + std::to_string(y);
    command += " " + std::to_string(z);
    command += " " + std::to_string(xx);
    command += " " + std::to_string(yy);
    command += " " + std::to_string(zz);

    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();
}

void SkillHandlerInterface::add_end()
{
    std::string command = "Stop";

    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();
}

void SkillHandlerInterface::set_default_behaviour()
{
    std::string command = "SetDefaulBehavior";

    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();
}

void SkillHandlerInterface::go_to_abs_pose(double x, double y, double z, double xx, double yy, double zz, double duration)
{
    Eigen::Matrix<double, 4, 4> goal_pose;

    goal_pose(0,3)= x;
    goal_pose(1,3)= y;
    goal_pose(2,3)= z;
    goal_pose(3,3)= 1;

    Eigen::Matrix3d n;
    n = Eigen::AngleAxisd(xx, Eigen::Vector3d::UnitX())
      * Eigen::AngleAxisd(yy, Eigen::Vector3d::UnitY())
      * Eigen::AngleAxisd(zz, Eigen::Vector3d::UnitZ());

    goal_pose(3,0)= 0;
    goal_pose(3,1)= 0;
    goal_pose(3,2)= 0;

    goal_pose.block(0,0,3,3)= n;

    std::cout << goal_pose << std::endl;

    std::string command = "AbsPose ";
    for(int i = 0; i < goal_pose.size(); i ++){
         command += std::to_string(goal_pose(i));
         command += " ";
    }
    command += std::to_string(duration);

    sequence_elements_.push_back(command);
    updateSequenceWidget();
    updateConfig();
}

void SkillHandlerInterface::setup_skillhandler(Videostream *videostream)
{
    skill_handler_ = new SkillHandler(videostream);
}

void SkillHandlerInterface::setSelectedGroup(QListWidgetItem *item)
{
    selected_group_name_ = item->text().toStdString();
}

void SkillHandlerInterface::setNewGroupName(QString item)
{
    new_group_name_ = item.toStdString();
}



void SkillHandlerInterface::updateGroupWidget()
{
    emit clearGroupWidget();

    for(auto& element : groups_){
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(element));
        emit addGroupToWidget(item);
    }
}

void SkillHandlerInterface::setNewSkillName(QString item)
{
    new_skill_name_ = item.toStdString();
}

void SkillHandlerInterface::addSkill()
{
    if (std::find(skills_.begin(), skills_.end(),new_skill_name_)==skills_.end()){
        skills_.push_back(new_skill_name_);
        updateSkillWidget();
    }
}

void SkillHandlerInterface::removeSkill()
{
    std::vector<std::string>::iterator it = std::find(skills_.begin(), skills_.end(), selected_skill_name_);
    skills_.erase(std::remove(skills_.begin(), skills_.end(), selected_skill_name_), skills_.end());

    updateSkillWidget();
    emit setSkillRow(it-skills_.begin()-1);


    YAML::Node skills_dict = ConfigHandler::getConfig("skills.yaml");

    std::vector<std::string> skills_from_dict = skills_dict[selected_group_name_]["Skills"].as<std::vector<std::string>>();
    skills_from_dict.erase(std::remove(skills_from_dict.begin(), skills_from_dict.end(), selected_skill_name_), skills_from_dict.end());
    skills_dict[selected_group_name_]["Skills"] = skills_from_dict;

    skills_dict[selected_group_name_].remove(selected_skill_name_);

    ConfigHandler::updateConfig(skills_dict,"skills.yaml");

    if(it!=skills_.begin()){
        selected_skill_name_ = *(it-1);
    }else{
        selected_skill_name_ = *(it);
    }


}

void SkillHandlerInterface::setSelectedSkill(QListWidgetItem *item)
{
    selected_skill_name_ = item->text().toStdString();
}

void SkillHandlerInterface::updateSkillWidget()
{
    emit clearSkillWidget();

    for(auto& element : skills_){
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(element));
        emit addSkillToWidget(item);
    }
}


void SkillHandlerInterface::openSkills(QListWidgetItem *item)
{
    YAML::Node devices_dict = ConfigHandler::getConfig("skills.yaml");
    if(devices_dict[selected_group_name_]["Skills"].size()!=0){
        skills_ = devices_dict[item->text().toStdString()]["Skills"].as<std::vector<std::string>>();
    }else{
        skills_.clear();
    }


    updateSkillWidget();
}

void SkillHandlerInterface::openSequence(QListWidgetItem *item)
{
    YAML::Node devices_dict = ConfigHandler::getConfig("skills.yaml");
    if(devices_dict[selected_group_name_][item->text().toStdString()].size()!=0){
        sequence_elements_ = devices_dict[selected_group_name_][item->text().toStdString()].as<std::vector<std::string>>();
    }else{
        sequence_elements_.clear();
    }
    updateSequenceWidget();
}

void SkillHandlerInterface::removeSequenceElement()
{
    std::vector<std::string>::iterator it = std::find(sequence_elements_.begin(), sequence_elements_.end(), selected_sequence_element_name_);
    sequence_elements_.erase(std::remove(sequence_elements_.begin(), sequence_elements_.end(), selected_sequence_element_name_), sequence_elements_.end());

    updateSequenceWidget();
    emit setSequenceElementRow(it-sequence_elements_.begin()-1);
    if(it!=sequence_elements_.begin()){
        selected_sequence_element_name_ = *(it-1);
    }else{
        selected_sequence_element_name_ = *(it);
    }

    updateConfig();

}

void SkillHandlerInterface::setSelectedSequenceElement(QListWidgetItem *item)
{
    selected_sequence_element_name_ = item->text().toStdString();
}

void SkillHandlerInterface::updateSequenceWidget()
{
    emit clearSequenceElementWidget();

    for(auto& element : sequence_elements_){
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(element));
        emit addSequenceElementToWidget(item);
    }


}

SkillHandlerInterface::SkillHandlerInterface(QObject *parent)
    : QObject(parent)
{

}
