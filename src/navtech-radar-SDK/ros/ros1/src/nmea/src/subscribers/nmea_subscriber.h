#include <ros/ros.h>

#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"

class Nmea_subscriber 
{
public:
    Nmea_subscriber(ros::NodeHandle& nh);

private:
    constexpr static int nmea_gprmc_queue_size{ 4 };
    constexpr static int nmea_gphdt_queue_size{ 4 };
    constexpr static int nmea_pashr_queue_size{ 4 };

    void gprmc_data_callback(const nmea_ros::nmea_gprmc_message::Ptr msg) const;
    void gphdt_data_callback(const nmea_ros::nmea_gphdt_message::Ptr msg) const;
    void pashr_data_callback(const nmea_ros::nmea_pashr_message::Ptr msg) const;

    ros::Subscriber nmea_gprmc_subscriber;
    ros::Subscriber nmea_gphdt_subscriber;
    ros::Subscriber nmea_pashr_subscriber;
};