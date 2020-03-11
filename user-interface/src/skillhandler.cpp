#include "skillhandler.h"

#include <bitset>

void SkillHandler::addGroup()
{
    groups_.push_back("New Group");
    updateGroupWidget();
}

void SkillHandler::changeGroupName(QListWidgetItem *item)
{

}

void SkillHandler::moveGroupUp()
{
     std::vector<std::string>::iterator it = std::find(groups_.begin(), groups_.end(), selected_group_name);

     if(it != groups_.begin()){
         std::string temp = *(it-1);
         *(it-1) = *it;
         *it = temp;
     }

    updateGroupWidget();
    emit setGroupRow(it-groups_.begin()-1);
}

void SkillHandler::removeGroup()
{
    groups_.erase(std::remove(groups_.begin(), groups_.end(), selected_group_name), groups_.end());

    updateGroupWidget();
}

void SkillHandler::moveGroup(QModelIndexList indexes)
{
    qDebug() << "indexes" << endl;
    qDebug() << indexes << endl;
}

void SkillHandler::showChange(QListWidgetItem *current, QListWidgetItem *previous)
{
    qDebug() << "current" << endl;
    qDebug() << current->text() << endl;

    if(previous != nullptr){
        qDebug() << "previous" << endl;
        qDebug() << previous->text() << endl;
    }

}

void SkillHandler::setSelectedGroup(QListWidgetItem *item)
{
    selected_group_name = item->text().toStdString();
}

void SkillHandler::updateGroupWidget()
{
    emit clearGroupWidget();

    for(auto& element : groups_){
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(element));
        item->setFlags(item->flags () | Qt :: ItemIsEditable);
        std::cout << item->text().toStdString()  << std::endl;
        std::cout << std::bitset<8>(item->flags())  << std::endl;
        emit addGroupToWidget(item);
    }
}

SkillHandler::SkillHandler(QObject *parent) : QObject(parent)
{
    // Setup teaching
    YAML::Node devices_dict = ConfigHandler::getConfig("skills.yaml");
    groups_ = devices_dict["Groups"].as<std::vector<std::string>>();

    updateGroupWidget();
}
