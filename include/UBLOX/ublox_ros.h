#ifndef UBLOX_ROS_H
#define UBLOX_ROS_H

#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/NavSatStatus.h>

#include "UBLOX/ublox.h"

#include "ublox/PosVelEcef.h"
#include "ublox/PositionVelocityTime.h"
#include "ublox/RelPos.h"
#include "ublox/SurveyStatus.h"
#include "ublox/Ephemeris.h"
#include "ublox/GlonassEphemeris.h"
#include "ublox/Observation.h"
#include "ublox/ObsVec.h"

#include "ublox/CfgValGet.h"
#include "ublox/CfgValDel.h"
#include "ublox/CfgValSet.h"

namespace ublox_ros
{

class UBLOX_ROS
{
public:
    UBLOX_ROS();
    ~UBLOX_ROS();

private:
    ublox::UBLOX* ublox_ = nullptr;

    ros::NodeHandle nh_;
    ros::NodeHandle nh_private_;

    ros::Publisher pvt_pub_;
    ros::Publisher survey_status_pub_;
    ros::Publisher relpos_pub_;
    ros::Publisher ecef_pub_;
    ros::Publisher nav_sat_fix_pub_;
    ros::Publisher nav_sat_status_pub_;
    ros::Publisher eph_pub_;
    ros::Publisher geph_pub_;
    ros::Publisher obs_pub_;

    ros::Publisher base_ecef_pub_;
    ros::Publisher base_pvt_pub_;

    /**
     * @brief Callback for filling a PosVelTime ROS message from a UBX callback
     */
    void pvtCB(const ublox::UBX_message_t &ubxmsg, uint8_t f9pID=0);
    /**
     * @brief Callback for filling a RelPos ROS message from a UBX callback
     */
    void relposCB(const ublox::UBX_message_t &ubx_msg, uint8_t f9pID=0);
    /**
     * @brief Callback for filling a PosVelEcef ROS message with position data from a UBX callback
     */
    void posECEFCB(const ublox::UBX_message_t &ubx_msg, uint8_t f9pID=0);
    /**
     * @brief Callback for filling a PosVelEcef ROS message with velocity data from a UBX callback
     */
    void velECEFCB(const ublox::UBX_message_t &ubx_msg, uint8_t f9pID=0);
    /**
     * @brief Callback for filling a SurveyStatus ROS message from a UBX callback
     */
    void svinCB(const ublox::UBX_message_t &ubx_msg, uint8_t f9pID=0);
    /**
     * @brief Callback for filling a Observation ROS message from a UBX callback
     */
    void obsCB(const ublox::UBX_message_t &ubx_msg, uint8_t f9pID=0);

    void ephCB(const Ephemeris& eph);
    void gephCB(const GlonassEphemeris& eph);

    bool cfgValGet(ublox::CfgValGet::Request &req, ublox::CfgValGet::Response &res);
    bool cfgValDel(ublox::CfgValDel::Request &req, ublox::CfgValDel::Response &res);
    bool cfgValSet(ublox::CfgValSet::Request &req, ublox::CfgValSet::Response &res);
    ros::ServiceServer cfg_val_get;
    ros::ServiceServer cfg_val_del_;
    ros::ServiceServer cfg_val_set_;

    uint32_t pos_tow_;
    uint32_t vel_tow_;
    uint32_t pvt_tow_;
    uint32_t pvt_week_;
    // int message_rate;

    double ned_1[3];
    double ned_2[3];
    bool arrow_flag = false;
    double arrow[7];

    ros::Subscriber sub1;
    ros::Subscriber sub2;

    ublox::PosVelEcef ecef_msg_;
    void cb_rov1(const ublox::RelPos &msg);
    void cb_rov2(const ublox::RelPos &msg);


    /**
     * @brief creates a callback for the ubx message type
     * @param cls uint8_t class code -- See ubx_defs.h
     * @param type uint8_t type within class -- see ubx_defs.h
     * @param functionaddress within UBLOX_ROS
     * @param pointer to object from which the function is called
     */
    template<class M> void createCallback(uint8_t cls, uint8_t type, 
        void(ublox_ros::UBLOX_ROS::*fp)(const M &msg, uint8_t), ublox_ros::UBLOX_ROS *obj, uint8_t f9pID=0)
    {
        do
        {
            auto trampoline = [obj, fp](uint8_t _class, uint8_t _type, const ublox::UBX_message_t &ubx_msg, uint8_t f9pID=0)
            {
                (obj->*fp)(ubx_msg, f9pID);
            };

            this->ublox_->registerUBXCallback(cls, type, trampoline);
        } while (0);
    };

    /**
     * @brief Slices into an iterable
     * @param iterable
     * @param xstart starting index (inclusive)
     * @param xend ending index (not inclusive)
     * @return a neatly sliced iterable [xstart, xend)
     */
    template <class T> T slice(T iteratable, int xstart, int xend) 
    {
        // Declare subvariable
        T subiterate = new T[xend-xstart];

        for(int i=xstart; i< xend; i++) 
        {
            subiterate[i-xstart] = iteratable[i];
        }
    }
    
};

}

#endif
