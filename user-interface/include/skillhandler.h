#ifndef SKILLHANDLER_H
#define SKILLHANDLER_H

#include <QObject>
#include <QListWidgetItem>
#include <QDebug>

#include "config_handler.h"

class SkillHandler: public QObject
{
    Q_OBJECT

signals:
    void addGroupToWidget(QListWidgetItem *item);
    void setGroupRow(int);
    void clearGroupWidget();

public:
    void addGroup();
    void changeGroupName(QListWidgetItem *item);
    void moveGroupUp();
    void removeGroup();
    void moveGroup(QModelIndexList indexes);
    void showChange(QListWidgetItem *current, QListWidgetItem *previous);
    void setSelectedGroup(QListWidgetItem *item);
    void updateGroupWidget();
    explicit SkillHandler(QObject *parent = nullptr);

private:
    std::vector<std::string> groups_;
    std::string selected_group_name;
};

#endif // SKILLHANDLER_H
