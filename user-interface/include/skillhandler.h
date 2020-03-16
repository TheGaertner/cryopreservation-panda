#ifndef SKILLHANDLER_H
#define SKILLHANDLER_H

#include <QObject>
#include <QListWidgetItem>
#include <QDebug>

#include <Eigen/Dense>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "serialization.h"
#include "marker.h"
#include "videostream.h"
#include "devicehandler.h"

class SkillHandler
{
public:
    void setActualMarker(QListWidgetItem *item);
    void setLastState(StateSerialization *state);
    void setActualPose();
    void setDuration(double time);

    std::string create_relative_pose();
    std::string go_to_relative_pose(std::string command);
    std::string go_to_absolute_pose( Eigen::Matrix<double, 4, 4> pose);
    std::string create_joint_position();

    explicit SkillHandler(Videostream* videostream, QObject *parent = nullptr);

private:
    StateSerialization lastState_;
    Videostream* videostream_;
    int actual_marker_id_;
    Eigen::Matrix<double, 4, 4> marker_position;
    Eigen::Matrix<double, 4, 4> initial_postion;
    Eigen::Matrix<double, 4, 4> goal_pose_;
    Eigen::Matrix<double, 7, 1> joint_pose_;
    double duration;


};

#endif // SKILLHANDLER_H
