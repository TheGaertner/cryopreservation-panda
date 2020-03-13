#ifndef SKILLHANDLER_H
#define SKILLHANDLER_H

#include <QObject>
#include <QListWidgetItem>
#include <QDebug>

#include <Eigen/Dense>

#include "serialization.h"

class SkillHandler : public QObject
{
    Q_OBJECT
public:
    void setActualMarker(QListWidgetItem *item);
    void setLastState(StateSerialization *state);
    void setActualPose();
    void setDuration(double time);

    std::string create_relative_pose();
    std::string create_joint_position();

    explicit SkillHandler(QObject *parent = nullptr);

private:
    StateSerialization lastState_;
    int actual_marker_;
    Eigen::Matrix<double, 4, 4> initial_postion;
    Eigen::Matrix<double, 4, 4> goal_pose_;
    Eigen::Matrix<double, 7, 1> joint_pose_;
    double duration;


};

#endif // SKILLHANDLER_H
