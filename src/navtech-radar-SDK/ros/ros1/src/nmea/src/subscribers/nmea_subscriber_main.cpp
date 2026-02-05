#include <ros/ros.h>

#include "nmea_subscriber.h"

#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"


int main(int argc, char* argv[])
{
    ros::init(argc, argv, "nmea_subscriber");
    ros::NodeHandle nh;
    Nmea_subscriber nmea_subscriber(nh);
    ros::spin();
}