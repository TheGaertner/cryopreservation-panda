#include "taskhandler.h"
#include <boost/algorithm/string.hpp>

TaskHandler::TaskHandler(SkillHandlerInterface *skill_handler_interface, QObject *parent)
    : QObject(parent)
{
    skill_handler_interface_ = skill_handler_interface;
}

void TaskHandler::setSelectedGroup(QListWidgetItem *item)
{
    selected_group_name_ = item->text().toStdString();
    openSkills(item);
}

void TaskHandler::openSkills(QListWidgetItem *item)
{
    YAML::Node devices_dict = ConfigHandler::getConfig("skills.yaml");
    if(devices_dict[selected_group_name_]["Skills"].size()!=0){
        skills_ = devices_dict[item->text().toStdString()]["Skills"].as<std::vector<std::string>>();
    }else{
        skills_.clear();
    }

    updateSkillWidget();
}

void TaskHandler::updateSkillWidget()
{
    for(auto& element : skills_){
        QListWidgetItem* itemTask = new QListWidgetItem(QString::fromStdString(element));
        emit addSkillToWidgetTask(itemTask);
    }
}

void TaskHandler::updateTaskWidget()
{
    emit clearTaskWidget();

    for(auto& element : tasks_){
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(element));
        emit addTaskToWidget(item);

        item = new QListWidgetItem(QString::fromStdString(element));
        emit addTaskToWidgetMain(item);
    }
}

void TaskHandler::addTask()
{
    if (std::find(tasks_.begin(), tasks_.end(),new_task_name_)==tasks_.end()){
        tasks_.push_back(new_task_name_);
        updateTaskWidget();
    }
}

void TaskHandler::setNewTaskName(QString item)
{
    new_task_name_ = item.toStdString();
}


void TaskHandler::removeTask()
{
    std::vector<std::string>::iterator it = std::find(tasks_.begin(), tasks_.end(), selected_task_name_);
    tasks_.erase(std::remove(tasks_.begin(), tasks_.end(), selected_task_name_), tasks_.end());

    updateTaskWidget();
    emit setTaskRow(it-tasks_.begin()-1);

    YAML::Node tasks_dict = ConfigHandler::getConfig("tasks.yaml");

    std::vector<std::string> tasks_from_dict = tasks_dict["Tasks"].as<std::vector<std::string>>();
    tasks_from_dict.erase(std::remove(tasks_from_dict.begin(), tasks_from_dict.end(), selected_task_name_), tasks_from_dict.end());
    tasks_dict["Tasks"] = tasks_from_dict;

    tasks_dict.remove(selected_task_name_);

    ConfigHandler::updateConfig(tasks_dict,"tasks.yaml");


}

void TaskHandler::setSelectedTask(QListWidgetItem *item)
{
    selected_task_name_ = item->text().toStdString();
}

void TaskHandler::init()
{
    YAML::Node devices_dict = ConfigHandler::getConfig("tasks.yaml");
    tasks_ = devices_dict["Tasks"].as<std::vector<std::string>>();


    updateTaskWidget();
}

void TaskHandler::addStep()
{
    std::string new_step_name_ = selected_group_name_ + "&" + selected_skill_name_;
    if (std::find(steps_.begin(), steps_.end(),new_step_name_)==steps_.end()){
        steps_.push_back(new_step_name_);
        updateStepsWidget();
        updateConfig();
    }
}

void TaskHandler::updateStepsWidget()
{
    emit clearStepsWidgete();

    for(auto& element : steps_){
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(element));
        emit addStepToWidget(item);

    }
}

void TaskHandler::setSelectedSkill(QListWidgetItem *item)
{
    selected_skill_name_ = item->text().toStdString();
}



void TaskHandler::openSteps(QListWidgetItem *item)
{
    YAML::Node devices_dict = ConfigHandler::getConfig("tasks.yaml");
    if(devices_dict[selected_task_name_].size()!=0){
        steps_ = devices_dict[item->text().toStdString()].as<std::vector<std::string>>();
    }else{
        steps_.clear();
    }


    updateStepsWidget();
}

void TaskHandler::updateConfig()
{
    YAML::Node tasks_dict = ConfigHandler::getConfig("tasks.yaml");
    std::string selected_step_name_ = selected_group_name_ + "&" + selected_skill_name_;

    tasks_dict[selected_task_name_]=steps_;

    if(tasks_dict[selected_task_name_].size()!=0){
        std::vector<std::string> steps_from_dict = tasks_dict[selected_task_name_].as<std::vector<std::string>>();
        if (std::find(steps_from_dict.begin(), steps_from_dict.end(),selected_step_name_)==steps_from_dict.end()){
            tasks_dict[selected_task_name_].push_back(selected_step_name_);
        }
    }else{
        tasks_dict[selected_task_name_].push_back(selected_step_name_);
    }


    std::vector<std::string> tasks_from_dict = tasks_dict["Tasks"].as<std::vector<std::string>>();
    if (std::find(tasks_from_dict.begin(), tasks_from_dict.end(),selected_task_name_)==tasks_from_dict.end()){
        tasks_dict["Tasks"].push_back(selected_task_name_);
    }


    ConfigHandler::updateConfig(tasks_dict,"tasks.yaml");
}

void TaskHandler::setMainTask(QListWidgetItem *item)
{
    main_task_name_ = item->text().toStdString();
}

void TaskHandler::executeMainTask(){
    skill_handler_interface_->clearRunningSequence();

    YAML::Node tasks_dict = ConfigHandler::getConfig("tasks.yaml");
    YAML::Node skills_dict = ConfigHandler::getConfig("skills.yaml");
    std::vector<std::string> steps_from_dict = tasks_dict[main_task_name_].as<std::vector<std::string>>();
    for(auto& element : steps_from_dict){
        std::vector<std::string> results;

        boost::algorithm::split(results, element, boost::is_any_of("&"));

        std::string group = results[0];
        std::string skill = results[1];

        std::vector<std::string> sequence = skills_dict[group][skill].as<std::vector<std::string>>();
        skill_handler_interface_->storeSequence(sequence);
    }

    skill_handler_interface_->playSequence();
}
