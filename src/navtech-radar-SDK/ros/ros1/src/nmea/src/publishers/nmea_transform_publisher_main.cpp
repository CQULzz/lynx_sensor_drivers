#include <chrono>
#include <thread>
#include <functional>
#include <memory>
#include <string>
#include <ros/ros.h>
#include "nmea_transform_publisher.h"


int main(int argc, char* argv[])
{
    ros::init(argc, argv, "nmea_transform_publisher");

    ros::NodeHandle nh;
    Nmea_transform_publisher nmea_transform_publisher(nh);
    ROS_INFO("Starting nmea transform publisher");

    ROS_INFO("Nmea transform publisher started");

    ros::spin();
    while (ros::ok()) {
        nmea_transform_publisher.send_transform();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    ROS_INFO("Stopping nmea transform publisher");
    ROS_INFO("Stopped nmea transform publisher");
}