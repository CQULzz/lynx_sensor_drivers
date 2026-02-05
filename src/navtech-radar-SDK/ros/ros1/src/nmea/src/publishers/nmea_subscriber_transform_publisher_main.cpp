#include <ros/ros.h>

#include "nmea_subscriber_transform_publisher.h"

#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"


int main(int argc, char* argv[])
{
    ros::init(argc, argv, "nmea_subscriber_transform_publisher");
    ros::NodeHandle nh;
    Nmea_subscriber_transform_publisher nmea_subscriber_transform_publisher(nh);
    // rclcpp::spin(node);
    ros::spin();
}