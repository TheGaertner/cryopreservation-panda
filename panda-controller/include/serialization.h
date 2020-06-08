#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#define UNUSED(x) (void)(x)


#include <boost/asio.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <ctime>

#include <franka/robot_state.h>



namespace boost {
namespace serialization {

template<class Archive, class T, size_t N>
void serialize(Archive & ar, std::array<T,N> & a, const unsigned int version)
{
    UNUSED(version);
    ar & boost::serialization::make_array(a.data(), a.size());
}

} // namespace serialization
} // namespace boost

class StateSerialization{

public:
    void update_state(franka::RobotState *state);
    void update_time(){current_time = time(NULL);}

    StateSerialization();
    StateSerialization(franka::RobotState *state);
    ~ StateSerialization ();


    franka::RobotState *state_;
    time_t current_time;
    int robot_mode_converted;
    std::array<double, 7> debug{};
    std::array<double, 6> debug2{};
    double getData(std::string name, int id = 99);
    static std::vector<std::string> getNames();

    std::string sender_ip;
    std::string message;
    int sender_port;

private:
    bool delete_state_ = false;


    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        UNUSED(version);
        // Simply list all the fields to be serialized/deserialized.
        ar & state_->O_T_EE;
        ar & state_->O_T_EE_d;
        ar & state_->q;
        ar & state_->dq_d;
        ar & state_->tau_J;
        ar & current_time;
        ar & state_->joint_contact;
        ar & state_->joint_collision;
        ar & state_->cartesian_contact;
        ar & state_->cartesian_collision;
        ar & state_->O_F_ext_hat_K;
        ar & state_->EE_T_K;
        ar & state_->tau_ext_hat_filtered;
        ar & state_->K_F_ext_hat_K;
        ar & state_->F_T_EE;
        ar & robot_mode_converted;
        ar & message;
    };


};


#endif // SERIALIZATION_H
