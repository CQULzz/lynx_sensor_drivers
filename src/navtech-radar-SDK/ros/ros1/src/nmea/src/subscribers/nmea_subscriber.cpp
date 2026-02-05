#include <functional>
#include <memory>
#include <stdexcept>
#include <exception>
#include <cassert>

#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"
#include "nmea_subscriber.h"


Nmea_subscriber::Nmea_subscriber(ros::NodeHandle& nh)
{
    nmea_gprmc_subscriber = 
    nh.subscribe(
        "nmea_data/gprmc_data",
        10,
        &Nmea_subscriber::gprmc_data_callback,
        this
    );

    nmea_gphdt_subscriber = 
    nh.subscribe(
        "nmea_data/gphdt_data",
        10,
        &Nmea_subscriber::gphdt_data_callback,
        this
    );

    nmea_pashr_subscriber = 
    nh.subscribe(
        "nmea_data/pashr_data",
        10,
        &Nmea_subscriber::pashr_data_callback,
        this
    );
}


void Nmea_subscriber::gprmc_data_callback(const nmea_ros::nmea_gprmc_message::Ptr msg) const
{
    ROS_INFO("\nGPRMC data recieved");

    ROS_INFO("%s", msg->message_id.c_str());
    ROS_INFO("%f", msg->position_utc);
    ROS_INFO("%d", msg->status);
    ROS_INFO("%f", msg->latitude);
    ROS_INFO("%d", msg->latitude_dir);
    ROS_INFO("%f", msg->longitude);
    ROS_INFO("%d", msg->longitude_dir);
    ROS_INFO("%f", msg->speed);
    ROS_INFO("%f", msg->angle);
    ROS_INFO("%s", msg->date.c_str());
    ROS_INFO("%f", msg->magnetic_variation);
    ROS_INFO("%d", msg->magnetic_variation_dir);
    ROS_INFO("%d", msg->mode_indicator);
    ROS_INFO("%s", msg->checksum_data.c_str());
}

void Nmea_subscriber::gphdt_data_callback(const nmea_ros::nmea_gphdt_message::Ptr msg) const
{
    ROS_INFO("\nGPHDT data recieved");

    ROS_INFO("%s", msg->message_id.c_str());
ROS_INFO("%f", msg->heading);
    ROS_INFO("%d", msg->heading_relative);
    ROS_INFO("%s", msg->checksum_data.c_str());
}


void Nmea_subscriber::pashr_data_callback(const nmea_ros::nmea_pashr_message::Ptr msg) const
{
    ROS_INFO("\nPASHR data recieved");

    ROS_INFO("%s", msg->message_id.c_str());
    ROS_INFO("%f", msg->utc_time);
    ROS_INFO("%f", msg->heading);
    ROS_INFO("%d", msg->true_heading_flag);
    ROS_INFO("%f", msg->roll);
    ROS_INFO("%f", msg->pitch);
    ROS_INFO("%f", msg->heave);
    ROS_INFO("%f", msg->roll_accuracy);
    ROS_INFO("%f", msg->pitch_accuracy);
    ROS_INFO("%f", msg->heading_accuracy);
    ROS_INFO("%u", msg->aiding_status);
    ROS_INFO("%u", msg->imu_status);
    ROS_INFO("%s", msg->checksum_data.c_str());
}