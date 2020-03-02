#include "serialization.h"


StateSerialization::StateSerialization(){
    state_ = new franka::RobotState();
    delete_state_ = true;

}

StateSerialization::StateSerialization(franka::RobotState *state) {
    state_ = state;
    delete_state_ = false;
}

StateSerialization::~StateSerialization()
{
    if(delete_state_){
        delete state_;
    }
}

void StateSerialization::update_state(franka::RobotState *state){
    state_ = state;
    delete_state_ = false;
}



double StateSerialization::getData(std::string name, int id)
{
    if(name == "O_T_EE"){
        if(id==99){
            return state_->O_T_EE.size();
        }
        return state_->O_T_EE[id];
    }
    if(name == "O_T_EE_d"){
        if(id==99){
            return state_->O_T_EE_d.size();
        }
        return state_->O_T_EE_d[id];
    }
    if(name == "F_T_EE"){
        if(id==99){
            return state_->F_T_EE.size();
        }
        return state_->F_T_EE[id];
    }
    if(name == "EE_T_K"){
        if(id==99){
            return state_->EE_T_K.size();
        }
        return state_->EE_T_K[id];
    }
    if(name == "m_ee"){
        if(id==99){
            return 1;
        }
        return state_->m_ee;
    }
    if(name == "I_ee"){
        if(id==99){
            return state_->I_ee.size();
        }
        return state_->I_ee[id];
    }
    if(name == "F_x_Cee"){
        if(id==99){
            return state_->F_x_Cee.size();
        }
        return state_->F_x_Cee[id];
    }
    if(name == "m_load"){
        if(id==99){
            return state_->m_load;
        }
        return state_->m_load;
    }
    if(name == "I_load"){
        if(id==99){
            return state_->I_load.size();
        }
        return state_->I_load[id];
    }
    if(name == "F_x_Cload"){
        if(id==99){
            return state_->F_x_Cload.size();
        }
        return state_->F_x_Cload[id];
    }
    if(name == "m_total"){
        if(id==99){
            return 1;
        }
        return state_->m_total;
    }
    if(name == "I_total"){
        if(id==99){
            return state_->I_total.size();
        }
        return state_->I_total[id];
    }
    if(name == "F_x_Ctotal"){
        if(id==99){
            return state_->F_x_Ctotal.size();
        }
        return state_->F_x_Ctotal[id];
    }
    if(name == "elbow"){
        if(id==99){
            return state_->elbow.size();
        }
        return state_->elbow[id];
    }
    if(name == "elbow_d"){
        if(id==99){
            return state_->elbow_d.size();
        }
        return state_->elbow_d [id];
    }
    if(name == "elbow_c"){
        if(id==99){
            return state_->elbow_c.size();
        }
        return state_->elbow_c[id];
    }
    if(name == "delbow_c"){
        if(id==99){
            return state_->delbow_c.size();
        }
        return state_->delbow_c[id];
    }
    if(name == "ddelbow_c"){
        if(id==99){
            return state_->ddelbow_c.size();
        }
        return state_->ddelbow_c.size();
    }
    if(name == "tau_J"){
        if(id==99){
            return state_->tau_J.size();
        }
        return state_->tau_J[id];
    }
    if(name == "tau_J_d"){
        if(id==99){
            return state_->tau_J_d.size();
        }
        return state_->tau_J_d[id];
    }
    if(name == "dtau_J"){
        if(id==99){
            return state_->dtau_J.size();
        }
        return state_->dtau_J[id];
    }
    if(name == "q"){
        if(id==99){
            return state_->q.size();
        }
        return state_->q[id];
    }
    if(name == "q_d"){
        if(id==99){
            return state_->q_d.size();
        }
        return state_->q_d[id];
    }
    if(name == "dq"){
        if(id==99){
            return state_->dq.size();
        }
        return state_->dq[id];
    }
    if(name == "dq_d"){
        if(id==99){
            return state_->dq_d.size();
        }
        return state_->dq_d[id];
    }
    if(name == "ddq_d"){
        if(id==99){
            return state_->ddq_d.size();
        }
        return state_->ddq_d[id];
    }
    if(name == "joint_contact"){
        if(id==99){
            return state_->joint_contact.size();
        }
        return state_->joint_contact[id];
    }
    if(name == "cartesian_contact"){
        if(id==99){
            return state_->cartesian_contact.size();
        }
        return state_->cartesian_contact[id];
    }
    if(name == "joint_collision"){
        if(id==99){
            return state_->joint_collision.size();
        }
        return state_->joint_collision[id];
    }
    if(name == "cartesian_collision"){
        if(id==99){
            return state_->cartesian_collision.size();
        }
        return state_->cartesian_collision[id];
    }
    if(name == "tau_ext_hat_filtered"){
        if(id==99){
            return state_->tau_ext_hat_filtered.size();
        }
        return state_->tau_ext_hat_filtered[id];
    }
    if(name == "O_F_ext_hat_K"){
        if(id==99){
            return state_->O_F_ext_hat_K.size();
        }
        return state_->O_F_ext_hat_K[id];
    }
    if(name == "O_dP_EE_d"){
        if(id==99){
            return state_->O_dP_EE_d.size();
        }
        return state_->O_dP_EE_d[id];
    }
    if(name == "O_T_EE_c"){
        if(id==99){
            return state_->O_T_EE_c.size();
        }
        return state_->O_T_EE_c[id];
    }
    if(name == "O_dP_EE_c"){
        if(id==99){
            return state_->O_dP_EE_c.size();
        }
        return state_->O_dP_EE_c[id];
    }
    if(name == "O_ddP_EE_c"){
        if(id==99){
            return state_->O_ddP_EE_c.size();
        }
        return state_->O_ddP_EE_c[id];
    }
    if(name == "theta"){
        if(id==99){
            return state_->theta.size();
        }
        return state_->theta[id];
    }
    if(name == "dtheta"){
        if(id==99){
            return state_->dtheta.size();
        }
        return state_->dtheta[id];
    }



}

std::vector<std::string> StateSerialization::getNames()
{
    std::vector<std::string> names = {"O_T_EE",
                                     "O_T_EE_d",
                                     "F_T_EE",
                                     "EE_T_K",
                                     "m_ee",
                                     "I_ee",
                                     "F_x_Cee",
                                     "m_load",
                                     "I_load",
                                     "F_x_Cload",
                                     "m_total",
                                     "I_total",
                                     "F_x_Ctotal",
                                     "elbow",
                                     "elbow_d",
                                     "elbow_c",
                                     "delbow_c",
                                     "ddelbow_c",
                                     "tau_J",
                                     "tau_J_d",
                                     "dtau_J",
                                     "q",
                                     "q_d",
                                     "dq",
                                     "dq_d",
                                     "ddq_d",
                                     "joint_contact",
                                     "cartesian_contact",
                                     "joint_collision",
                                     "cartesian_collision",
                                     "tau_ext_hat_filtered",
                                     "O_F_ext_hat_K",
                                     "K_F_ext_hat_K",
                                     "O_dP_EE_d",
                                     "O_T_EE_c",
                                     "O_dP_EE_c",
                                     "O_ddP_EE_c",
                                     "theta",
                                     "dtheta"};
    return names;
}
