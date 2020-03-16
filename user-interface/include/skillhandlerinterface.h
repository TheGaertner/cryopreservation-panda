#ifndef SKILLHANDLERINTERFACE_H
#define SKILLHANDLERINTERFACE_H

#include <QObject>
#include <QListWidgetItem>
#include <QDebug>

#include "config_handler.h"
#include "skillhandler.h"
#include "serialization.h"
#include "videostream.h"

class SkillHandlerInterface: public QObject
{
    Q_OBJECT

signals:
    void addGroupToWidget(QListWidgetItem *item);
    void setGroupRow(int);
    void clearGroupWidget();

    void addSkillToWidget(QListWidgetItem *item);
    void setSkillRow(int);
    void clearSkillWidget();

    void addSequenceElementToWidget(QListWidgetItem *item);
    void setSequenceElementRow(int);
    void clearSequenceElementWidget();

    void addMovementPrimitiveToWidget(QStringList item);

    void command(QString command);

public:
    void setSelectedGroup(QListWidgetItem *item);
    void setNewGroupName(QString item);
    void addGroup();
    void removeGroup();

    void updateGroupWidget();

    void setNewSkillName(QString item);
    void addSkill();
    void removeSkill();
    void setSelectedSkill(QListWidgetItem *item);

    void updateSkillWidget();

    void openSkills(QListWidgetItem *item);

    void openSequence(QListWidgetItem *item);

    void removeSequenceElement();
    void setSelectedSequenceElement(QListWidgetItem *item);

    void updateSequenceWidget();

    void moveSequenceElementUp();
    void moveSequenceElementDown();

    void init();
    void updateConfig();
    void playSequence();

    void setActualMarker(QListWidgetItem *item){skill_handler_->setActualMarker(item);};
    void setLastState(StateSerialization *state){skill_handler_->setLastState(state);};
    void savePose(){skill_handler_->setActualPose();};
    void saveDuration(QString time){skill_handler_->setDuration(time.toDouble());};

    void createRelativePose();
    void createJointPosition();
    void createRelMovement(double x, double y, double z, double xx, double yy, double zz, double duration, bool EE_frame);
    void gripper_action(double width, double speed, double force, double tolerance);
    void gripper_homing();
    void set_colcon_behaviour(double col,double con);
    void set_cartesian_impedance(double x, double y, double z, double xx, double yy, double zz);
    void add_end();
    void set_default_behaviour();

    void setup_skillhandler(Videostream* videostream);

    explicit SkillHandlerInterface(QObject *parent = nullptr);

private:
    SkillHandler *skill_handler_;

    std::vector<std::string> groups_;
    std::string selected_group_name_;
    std::string new_group_name_;

    std::vector<std::string> skills_;
    std::string selected_skill_name_;
    std::string new_skill_name_;

    std::vector<std::string> sequence_elements_;
    std::string selected_sequence_element_name_;
    //std::string new_sequence_element_name_;
};

#endif // SKILLHANDLERINTERFACE_H
