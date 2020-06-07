#ifndef TASKHANDLER_H
#define TASKHANDLER_H

#include <QObject>
#include <QListWidgetItem>
#include <QDebug>

#include "config_handler.h"
#include "skillhandlerinterface.h"

class TaskHandler: public QObject
{
    Q_OBJECT

signals:
    void addSkillToWidgetTask(QListWidgetItem *item);
    void clearTaskWidget();
    void addTaskToWidget(QListWidgetItem *item);
    void setTaskRow(int);
    void clearStepsWidgete();
    void addStepToWidget(QListWidgetItem *item);
    void addTaskToWidgetMain(QListWidgetItem *item);

public:
    //    void setSelectedGroup(QListWidgetItem *item);
    //    void openSkills(QListWidgetItem *item);
    void openSkills(QListWidgetItem *item);
    void updateSkillWidget();
    void updateTaskWidget();
    void setSelectedGroup(QListWidgetItem *item);
    void addTask();
    void setNewTaskName(QString item);

    explicit TaskHandler(SkillHandlerInterface* skill_handler_interface, QObject *parent = nullptr);


    void setSelectedTask(QListWidgetItem *item);
    void init();
    void removeTask();
    void addStep();
    void updateStepsWidget();
    void setSelectedSkill(QListWidgetItem *item);
    void openSteps(QListWidgetItem *item);
    void updateConfig();
    void setMainTask(QListWidgetItem *item);
    void executeMainTask();
private:
    std::vector<std::string> skills_;
    std::string selected_group_name_;

    std::vector<std::string> tasks_;
    std::string new_task_name_;
    std::string selected_task_name_;
    std::string selected_skill_name_;
    std::vector<std::string> steps_;
    std::string main_task_name_;
    SkillHandlerInterface*skill_handler_interface_;

};

#endif // TASKHANDLER_H
