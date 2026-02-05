#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <ros/ros.h>

#include "nmea_publisher.h"


int main(int argc, char* argv[])
{
    ros::init(argc, argv, "nmea_publisher");
    ros::NodeHandle nh;
    Nmea_publisher nmea_publisher(nh);
    ROS_INFO("Starting nmea client");
    
    while (ros::ok()) {
        ros::spinOnce();
    }

    ROS_INFO("Stopping nmea client");
    ROS_INFO("Stopped nmea client");
}