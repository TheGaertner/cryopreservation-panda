#ifndef TASKHANDLER_H
#define TASKHANDLER_H

#include <QObject>
#include <QListWidgetItem>
#include <QDebug>

class TaskHandler: public QObject
{
    Q_OBJECT
public:
    //    void setSelectedGroup(QListWidgetItem *item);
    //    void openSkills(QListWidgetItem *item);

    explicit TaskHandler(QObject *parent = nullptr);


private:

};

#endif // TASKHANDLER_H
